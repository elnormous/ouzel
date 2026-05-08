#include "ViewportPanel.hpp"

#include <algorithm>
#include "imgui.h"
#include "../../engine/core/Engine.hpp"
#include "../../engine/graphics/Flags.hpp"
#include "../../engine/graphics/Graphics.hpp"
#include "../../engine/graphics/PixelFormat.hpp"
#include "../../engine/gui/imgui/ImGuiContext.hpp"
#include "../../engine/scene/Layer.hpp"

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
                                 scene::Layer& hostLayer):
        Panel{"Viewport", true},
        engine_{eng},
        imgui_{imgui}
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

        ImGui::Image(static_cast<ImTextureID>(handle), avail);

        ImGui::End();
    }
}
