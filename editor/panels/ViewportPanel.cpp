#include "ViewportPanel.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include "imgui.h"
#include "../../engine/assets/Asset.hpp"
#include "../../engine/core/Engine.hpp"
#include "../../engine/graphics/Flags.hpp"
#include "../../engine/graphics/Graphics.hpp"
#include "../../engine/graphics/PixelFormat.hpp"
#include "../../engine/gui/imgui/ImGuiContext.hpp"
#include "../../engine/scene/Layer.hpp"
#include "../../engine/scene/Scene.hpp"
#include "../CommandStack.hpp"
#include "../EditorPicking.hpp"
#include "../SelectionContext.hpp"
#include "../commands/AssetCommands.hpp"

namespace fluxion::editor
{
    namespace
    {
        // Fixed render-target dimension. Picked large enough that
        // typical editor panel sizes don't visibly suffer from the
        // bilinear stretch ImGui::Image performs to fit the panel,
        // and small enough to keep memory + per-frame fill cost cheap.
        //
        // We DELIBERATELY do not recreate the texture when the panel
        // changes size: that path would queue InitTexture commands
        // from the render thread (the imgui draw callback) into
        // Graphics::commandBuffer, which the main thread is also
        // writing — a textbook data race that produced freed-SRV
        // crashes inside ImGui_ImplDX11_RenderDrawData when the user
        // dragged the panel border. Pan / zoom work via camera
        // settings (independent of the RT size) so a fixed RT loses
        // nothing functional, just a small amount of crispness when
        // the panel is much larger than this dimension.
        constexpr std::uint32_t kRenderTargetDimension = 1024U;
    }

    ViewportPanel::ViewportPanel(core::Engine& eng,
                                 gui::imgui::Context& imgui,
                                 scene::Layer& hostLayer,
                                 scene::Scene& workingScene,
                                 SelectionContext& selection,
                                 const scene::Layer* excludeLayer,
                                 CommandStack* commandStack):
        Panel{"Viewport", true},
        engine_{eng},
        imgui_{imgui},
        workingScene_{workingScene},
        selection_{selection},
        excludeLayer_{excludeLayer},
        userLayer_{&hostLayer},
        commandStack_{commandStack}
    {
        // Tell the gizmo where to push transform commands when it
        // sees a drag complete. The host layer is also where the
        // commands look up the actor by uuid for redo/undo.
        gizmo_.setCommandTarget(commandStack, &hostLayer);
        // Order matters: set up the render target FIRST, then the
        // camera, then add the camera to its host layer. RTSample
        // follows the same recipe — never let the engine see a camera
        // attached to a populated layer with a stale (or null) RT.
        recreateTargets(math::Size<std::uint32_t, 2>{
            kRenderTargetDimension, kRenderTargetDimension});

        // Mirror the back-buffer-clear camera so the embedded view
        // matches what the user expects. The user can override these
        // through the Inspector once the camera-actor selection exists
        // for them — but the actor itself is hidden from the tree, so
        // tuning is a future-phase concern.
        camera_.setClearColorBuffer(true);
        camera_.setClearDepthBuffer(true);
        camera_.setClearColor(math::Color{32, 32, 40, 255});
        // exactFit (NOT showAll): stretches the world rectangle to
        // fill the SQUARE render target anisotropically. ImGui::Image
        // then stretches the square RT to the (typically rectangular)
        // panel, exactly cancelling out the per-axis distortion when
        // we set targetContentSize aspect = panel aspect every frame
        // (see draw()). With showAll the camera kept the world aspect
        // intact inside the square RT, but the RT-to-panel stretch
        // re-broke it — hence sprites coming out wide on a wide panel.
        camera_.setScaleMode(scene::Camera::ScaleMode::exactFit);
        camera_.setTargetContentSize(math::Size<float, 2>{60.0F, 60.0F});

        cameraActor_.setName("<viewport camera>");
        cameraActor_.addComponent(camera_);
        hostLayer.addChild(cameraActor_);
    }

    ViewportPanel::~ViewportPanel() = default;

    void ViewportPanel::recreateTargets(const math::Size<std::uint32_t, 2>& size)
    {
        if (size.v[0] == 0 || size.v[1] == 0) return;

        // Color: rgba8 sampled by ImGui::Image. bindShader is required
        // so the per-backend handle accessor (which goes through the
        // SRV / GLuint / id<MTLTexture>) actually returns something.
        colorTexture_ = std::make_unique<graphics::Texture>(
            engine_.getGraphics(),
            size,
            graphics::Flags::bindRenderTarget | graphics::Flags::bindShader,
            /*mipmaps*/ 1, /*sampleCount*/ 1,
            graphics::PixelFormat::rgba8UnsignedNorm);

        depthTexture_ = std::make_unique<graphics::Texture>(
            engine_.getGraphics(),
            size,
            graphics::Flags::bindRenderTarget | graphics::Flags::bindShader,
            /*mipmaps*/ 1, /*sampleCount*/ 1,
            graphics::PixelFormat::depth);

        renderTarget_ = std::make_unique<graphics::RenderTarget>(
            engine_.getGraphics(),
            std::vector<graphics::Texture*>{colorTexture_.get()},
            depthTexture_.get());

        camera_.setRenderTarget(renderTarget_.get());
        currentSize_ = size;
    }

    void ViewportPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(380, 60), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        // Toolbar above the image: mode buttons (T/R/S) + a Local/
        // World space toggle. Keyboard shortcuts (Q/W/E) still work
        // when the viewport window has focus, mirroring Unity's
        // bindings. The active mode button gets a tinted background
        // so the user can see at a glance which manipulator is on.
        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Q, /*repeat*/ false))
                gizmo_.setMode(Gizmo::Mode::Translate);
            if (ImGui::IsKeyPressed(ImGuiKey_W, /*repeat*/ false))
                gizmo_.setMode(Gizmo::Mode::Rotate);
            if (ImGui::IsKeyPressed(ImGuiKey_E, /*repeat*/ false))
                gizmo_.setMode(Gizmo::Mode::Scale);
        }

        auto modeButton = [&](const char* label, Gizmo::Mode mode) {
            const bool active = gizmo_.getMode() == mode;
            if (active)
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      ImVec4(0.30F, 0.55F, 0.85F, 1.0F));
            if (ImGui::Button(label))
                gizmo_.setMode(mode);
            if (active)
                ImGui::PopStyleColor();
            ImGui::SameLine();
        };
        modeButton("Translate", Gizmo::Mode::Translate);
        modeButton("Rotate",    Gizmo::Mode::Rotate);
        modeButton("Scale",     Gizmo::Mode::Scale);

        ImGui::Dummy(ImVec2(12.0F, 0.0F));
        ImGui::SameLine();

        const bool localSpace = gizmo_.getSpace() == Gizmo::Space::Local;
        if (ImGui::Button(localSpace ? "Local" : "World"))
            gizmo_.setSpace(localSpace ? Gizmo::Space::World
                                       : Gizmo::Space::Local);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(
                "Reference frame for translate / rotate handles.\n"
                "Scale always edits the actor's local components.");

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x < 1.0F || avail.y < 1.0F)
        {
            ImGui::End();
            return;
        }

        // Match the camera's content aspect to the panel's aspect.
        // The camera renders in exactFit mode, anisotropically
        // stretching the (panelAspect * H, H) world rectangle to the
        // square render-target. ImGui::Image then stretches the RT
        // to the (panelW, panelH) panel — the X stretch is panelW/1024
        // and the Y stretch is panelH/1024, so a unit world square
        // becomes (panelW/(panelAspect*H), panelH/H) = (panelH/H,
        // panelH/H) panel pixels. The panelAspect cancels out and
        // sprites display at correct aspect regardless of panel shape.
        // contentHeight_ is the user-controlled "zoom" parameter;
        // the wheel modifies that, the width follows from aspect.
        const float panelAspect = avail.x / avail.y;
        camera_.setTargetContentSize(math::Size<float, 2>{
            contentHeight_ * panelAspect, contentHeight_});

        // Skip the Image draw the very first frame after creation —
        // the engine's render loop hasn't filled the texture yet, so
        // sampling it would show whatever was in GPU memory. By the
        // second frame the back-buffer-clear camera has run and the
        // texture has valid contents.
        if (!colorTexture_)
        {
            ImGui::TextDisabled("(initializing render target...)");
            ImGui::End();
            return;
        }

        const std::uint64_t handle = imgui_.textureNativeHandle(*colorTexture_);
        if (handle == 0)
        {
            ImGui::TextDisabled("(GPU texture not ready)");
            ImGui::End();
            return;
        }

        // ImGui::Image is a *passive* widget — it draws but does not
        // register as a clickable item. The window-move logic then
        // sees clicks on the image area as "click on empty space" and
        // happily drags the panel along when the user grabs a gizmo
        // handle or clicks-to-pick. Stacking an InvisibleButton over
        // the same rect makes ImGui treat the area as an interactive
        // item, so window-drag is suppressed and IsItemClicked /
        // IsItemHovered work as one would expect.
        const ImVec2 cursorBeforeImage = ImGui::GetCursorScreenPos();
        ImGui::Image(static_cast<ImTextureID>(handle), avail);
        ImGui::SetCursorScreenPos(cursorBeforeImage);
        ImGui::InvisibleButton("##viewport_input", avail,
                               ImGuiButtonFlags_MouseButtonLeft);

        // Drop-to-create: dragging an asset from the AssetBrowser onto
        // the viewport area spawns a new actor at the cursor's world
        // position. The drop is undoable via CommandStack (single
        // CreateActorWithSpriteCommand step, mirrors the gizmo's
        // pattern). Only sprite + image types produce a renderable
        // result with a SpriteRenderer; other types (sound, font,
        // mesh) are accepted silently as no-ops for now — phase 13/14
        // can wire them up if needed.
        if (ImGui::BeginDragDropTarget())
        {
            if (const auto* payload =
                    ImGui::AcceptDragDropPayload("FLUXION_ASSET_REF"))
            {
                if (payload->DataSize == static_cast<int>(sizeof(assets::AssetReference)))
                {
                    assets::AssetReference incoming;
                    std::memcpy(&incoming, payload->Data,
                                sizeof(assets::AssetReference));

                    const bool isSpriteish =
                        incoming.type == assets::Asset::Type::sprite ||
                        incoming.type == assets::Asset::Type::image;

                    if (isSpriteish && userLayer_ && commandStack_)
                    {
                        // Convert the drop position from screen-space
                        // to the camera's world space. Cursor coords
                        // come from ImGui::GetMousePos because the
                        // payload is delivered on release; the
                        // panel's own mouseNorm computed below would
                        // have raced.
                        const ImVec2 dropPos = ImGui::GetMousePos();
                        // imgMin/imgW/imgH are computed below — use the
                        // already-known cursor + avail here so we don't
                        // need to forward-reference them.
                        const math::Vector<float, 2> dropNorm{
                            (dropPos.x - cursorBeforeImage.x) / avail.x,
                            (dropPos.y - cursorBeforeImage.y) / avail.y
                        };
                        const auto world = camera_.convertNormalizedToWorld(dropNorm);

                        commandStack_->execute(
                            std::make_unique<commands::CreateActorWithSpriteCommand>(
                                userLayer_, incoming,
                                math::Vector<float, 3>{world},
                                std::string{"SpriteActor"},
                                &selection_));
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Image rect in absolute screen coords. Used both to convert
        // a click into normalized [0,1] viewport coords (for picking)
        // and to project actor AABBs into ImGui pixel space (for the
        // overlay). Both must use exactly this rect — pulling it from
        // ImGui::GetItemRectMin/Max guarantees they line up with the
        // pixels the user actually sees, including DPI / zoom paths.
        // Reads the InvisibleButton's rect, which matches the image
        // (we set the cursor back before placing it).
        const ImVec2 imgMin = ImGui::GetItemRectMin();
        const ImVec2 imgMax = ImGui::GetItemRectMax();
        const float imgW = imgMax.x - imgMin.x;
        const float imgH = imgMax.y - imgMin.y;

        if (imgW > 0.0F && imgH > 0.0F)
        {
            const math::Vector<float, 2> rectMin{imgMin.x, imgMin.y};
            const math::Vector<float, 2> rectMax{imgMax.x, imgMax.y};

            // Camera navigation: middle-mouse drag pans, wheel zooms.
            // Both run before the gizmo / pick path so they can claim
            // the mouse if needed.
            const ImVec2 mouse = ImGui::GetMousePos();
            const math::Vector<float, 2> mouseNorm{
                (mouse.x - imgMin.x) / imgW,
                (mouse.y - imgMin.y) / imgH
            };
            const bool itemHovered = ImGui::IsItemHovered();

            // Pan: capture per-normalized world deltas at drag-start
            // so the cursor's world anchor stays put even if multiple
            // frames pass with mouse held but no movement.
            if (!panning_ &&
                itemHovered &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
            {
                panning_ = true;
                panStartCameraPos_ = cameraActor_.getPosition();
                panStartNorm_ = mouseNorm;
                const auto worldA = camera_.convertNormalizedToWorld(
                    math::Vector<float, 2>{0.0F, 0.0F});
                const auto worldB = camera_.convertNormalizedToWorld(
                    math::Vector<float, 2>{1.0F, 0.0F});
                const auto worldC = camera_.convertNormalizedToWorld(
                    math::Vector<float, 2>{0.0F, 1.0F});
                panWorldPerNormX_ = math::Vector<float, 3>{
                    worldB.v[0] - worldA.v[0],
                    worldB.v[1] - worldA.v[1],
                    worldB.v[2] - worldA.v[2]};
                panWorldPerNormY_ = math::Vector<float, 3>{
                    worldC.v[0] - worldA.v[0],
                    worldC.v[1] - worldA.v[1],
                    worldC.v[2] - worldA.v[2]};
            }
            if (panning_)
            {
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Middle))
                {
                    panning_ = false;
                }
                else
                {
                    const float dnx = mouseNorm.v[0] - panStartNorm_.v[0];
                    const float dny = mouseNorm.v[1] - panStartNorm_.v[1];
                    // Camera moves OPPOSITE to the cursor's world
                    // displacement so the world point under the
                    // start-of-drag cursor stays under the current
                    // cursor.
                    cameraActor_.setPosition(math::Vector<float, 3>{
                        panStartCameraPos_.v[0] -
                            (panWorldPerNormX_.v[0] * dnx +
                             panWorldPerNormY_.v[0] * dny),
                        panStartCameraPos_.v[1] -
                            (panWorldPerNormX_.v[1] * dnx +
                             panWorldPerNormY_.v[1] * dny),
                        panStartCameraPos_.v[2] -
                            (panWorldPerNormX_.v[2] * dnx +
                             panWorldPerNormY_.v[2] * dny)
                    });
                }
            }

            // Zoom: wheel scales contentHeight_ by 1.1× per notch.
            // The actual targetContentSize is recomputed at the top
            // of the next frame from contentHeight_ * panelAspect, so
            // the aspect-fit logic stays in one place.
            const float wheel = ImGui::GetIO().MouseWheel;
            if (itemHovered && wheel != 0.0F)
                contentHeight_ = std::max(
                    1.0F, contentHeight_ * std::pow(1.1F, -wheel));

            // AABB outlines first so the gizmo sits on top of them.
            drawAabbOverlay(workingScene_, camera_,
                            selection_.getSelected(),
                            rectMin, rectMax,
                            excludeLayer_,
                            &cameraActor_);

            // Gizmo runs before pick: if it claims the mouse (start of
            // a drag, or drag in progress), the click MUST NOT also
            // re-pick the actor under the cursor. The return value
            // gates the pick path.
            const bool gizmoUsedMouse = gizmo_.update(
                selection_.getSelected(), camera_, rectMin, rectMax);

            if (!gizmoUsedMouse &&
                ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                // Reuses mouseNorm computed at the start of the
                // input section; the click test fires when the mouse
                // is on the image, so the normalized form is valid.
                scene::Actor* hit = pickActor(workingScene_, camera_,
                                              mouseNorm,
                                              excludeLayer_,
                                              &cameraActor_);
                selection_.setSelected(hit);
            }
        }

        ImGui::End();
    }
}
