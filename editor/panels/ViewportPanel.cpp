#include "ViewportPanel.hpp"

#include <algorithm>
#include "imgui.h"
#include "../../engine/core/Engine.hpp"
#include "../../engine/graphics/Flags.hpp"
#include "../../engine/graphics/Graphics.hpp"
#include "../../engine/graphics/PixelFormat.hpp"
#include "../../engine/gui/imgui/ImGuiContext.hpp"
#include "../../engine/scene/Layer.hpp"
#include "../../engine/scene/Scene.hpp"
#include "../EditorPicking.hpp"
#include "../SelectionContext.hpp"

namespace fluxion::editor
{
    namespace
    {
        // Initial fallback target size. ImGui doesn't tell us the panel
        // dimensions until after the first Begin/End pair has been
        // submitted, so we allocate a reasonable default and re-create
        // when the panel reports its real size on the second frame.
        constexpr std::uint32_t kInitialDimension = 512U;

        // Don't churn on tiny resize jitter (ImGui fudges sub-pixel
        // values); only recreate when the request differs by at least
        // this many pixels in either axis.
        constexpr std::uint32_t kResizeHysteresis = 4U;
    }

    ViewportPanel::ViewportPanel(core::Engine& eng,
                                 gui::imgui::Context& imgui,
                                 scene::Layer& hostLayer,
                                 scene::Scene& workingScene,
                                 SelectionContext& selection,
                                 const scene::Layer* excludeLayer):
        Panel{"Viewport", true},
        engine_{eng},
        imgui_{imgui},
        workingScene_{workingScene},
        selection_{selection},
        excludeLayer_{excludeLayer}
    {
        // Order matters: set up the render target FIRST, then the
        // camera, then add the camera to its host layer. RTSample
        // follows the same recipe — never let the engine see a camera
        // attached to a populated layer with a stale (or null) RT.
        recreateTargets(math::Size<std::uint32_t, 2>{
            kInitialDimension, kInitialDimension});

        // Mirror the back-buffer-clear camera so the embedded view
        // matches what the user expects. The user can override these
        // through the Inspector once the camera-actor selection exists
        // for them — but the actor itself is hidden from the tree, so
        // tuning is a future-phase concern.
        camera_.setClearColorBuffer(true);
        camera_.setClearDepthBuffer(true);
        camera_.setClearColor(math::Color{32, 32, 40, 255});
        camera_.setScaleMode(scene::Camera::ScaleMode::showAll);
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

        // Tiny header strip above the image showing the active gizmo
        // mode. Q = translate, W = rotate, E = scale (Unity-style).
        // 5c-4 will turn this into a proper toolbar with clickable
        // buttons + a local/world-space toggle.
        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Q, /*repeat*/ false))
                gizmo_.setMode(Gizmo::Mode::Translate);
            if (ImGui::IsKeyPressed(ImGuiKey_W, /*repeat*/ false))
                gizmo_.setMode(Gizmo::Mode::Rotate);
            if (ImGui::IsKeyPressed(ImGuiKey_E, /*repeat*/ false))
                gizmo_.setMode(Gizmo::Mode::Scale);
        }
        const char* modeLabel =
            gizmo_.getMode() == Gizmo::Mode::Translate ? "Translate (Q)"
            : gizmo_.getMode() == Gizmo::Mode::Rotate ? "Rotate (W)"
            : "Scale (E)";
        ImGui::TextDisabled("Gizmo: %s", modeLabel);

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        if (avail.x < 1.0F || avail.y < 1.0F)
        {
            ImGui::End();
            return;
        }

        // Resize the render target if ImGui's content area changed
        // meaningfully. Hysteresis avoids re-creating on sub-pixel jitter.
        const auto requested = math::Size<std::uint32_t, 2>{
            static_cast<std::uint32_t>(avail.x),
            static_cast<std::uint32_t>(avail.y)
        };
        const auto deltaW = (requested.v[0] > currentSize_.v[0])
            ? requested.v[0] - currentSize_.v[0]
            : currentSize_.v[0] - requested.v[0];
        const auto deltaH = (requested.v[1] > currentSize_.v[1])
            ? requested.v[1] - currentSize_.v[1]
            : currentSize_.v[1] - requested.v[1];
        if (deltaW >= kResizeHysteresis || deltaH >= kResizeHysteresis)
            recreateTargets(requested);

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
                const ImVec2 mouse = ImGui::GetMousePos();
                const math::Vector<float, 2> normalized{
                    (mouse.x - imgMin.x) / imgW,
                    (mouse.y - imgMin.y) / imgH
                };
                scene::Actor* hit = pickActor(workingScene_, camera_,
                                              normalized,
                                              excludeLayer_,
                                              &cameraActor_);
                selection_.setSelected(hit);
            }
        }

        ImGui::End();
    }
}
