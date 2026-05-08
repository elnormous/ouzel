#ifndef FLUXION_EDITOR_VIEWPORTPANEL_HPP
#define FLUXION_EDITOR_VIEWPORTPANEL_HPP

#include <memory>
#include "../Gizmo.hpp"
#include "../Panel.hpp"
#include "../../engine/graphics/RenderTarget.hpp"
#include "../../engine/graphics/Texture.hpp"
#include "../../engine/math/Size.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Camera.hpp"

namespace fluxion::core
{
    class Engine;
}

namespace fluxion::gui::imgui
{
    class Context;
}

namespace fluxion::scene
{
    class Layer;
    class Scene;
}

namespace fluxion::editor
{
    class SelectionContext;

    // ImGui-hosted scene viewport.
    //
    // Owns its own camera + render target + color/depth textures so the
    // working scene draws into a GPU-side image instead of straight to
    // the back buffer. Each frame the panel asks ImGui for the content
    // region size, queries the per-backend texture handle through the
    // imgui::Context, and shows the image with ImGui::Image. The user
    // gets a dockable, movable, resizable scene view that looks like
    // any other panel.
    //
    // The viewport camera is added to a host scene::Layer (typically
    // the user's working layer) at construction so it sees the same
    // actors the editor lets the user manipulate. Its actor pointer is
    // exposed via getCameraActor() so EditorApp can hide it from the
    // Scene tree, the AABB overlay, and click-to-pick — the camera is
    // editor infrastructure, not user-authored content.
    //
    // Cross-platform: relies only on graphics::Texture, RenderTarget,
    // and the imgui::Context's textureNativeHandle() abstraction. The
    // per-renderer plumbing lives entirely in the engine; this panel
    // is identical on D3D11, OpenGL, and Metal.
    class ViewportPanel final : public Panel
    {
    public:
        ViewportPanel(core::Engine& eng,
                      gui::imgui::Context& imgui,
                      scene::Layer& hostLayer,
                      scene::Scene& workingScene,
                      SelectionContext& selection,
                      const scene::Layer* excludeLayer);
        ~ViewportPanel() override;

        ViewportPanel(const ViewportPanel&) = delete;
        ViewportPanel& operator=(const ViewportPanel&) = delete;

        void draw() override;

        // The actor that hosts the viewport's camera. Editor surfaces
        // (SceneTreePanel, AABB overlay, picking) must hide this actor
        // from the user — it's an editor implementation detail.
        const scene::Actor* getCameraActor() const noexcept { return &cameraActor_; }
        scene::Camera& getCamera() noexcept { return camera_; }

    private:
        // Allocates color + depth textures and a render target at
        // `size`, points the camera at the new RT, and stores the
        // current size. Safe to call while a previous RT exists; the
        // old textures get dropped on assignment.
        void recreateTargets(const math::Size<std::uint32_t, 2>& size);

        core::Engine& engine_;
        gui::imgui::Context& imgui_;
        scene::Scene& workingScene_;
        SelectionContext& selection_;
        const scene::Layer* excludeLayer_;

        scene::Camera camera_;
        scene::Actor cameraActor_;

        std::unique_ptr<graphics::Texture> colorTexture_;
        std::unique_ptr<graphics::Texture> depthTexture_;
        std::unique_ptr<graphics::RenderTarget> renderTarget_;

        // Translate gizmo for the currently-selected actor. Held by
        // value because its drag state lives across frames.
        Gizmo gizmo_;

        // Pan / zoom state. When the user middle-mouse-drags inside
        // the viewport the camera shifts so the world point under the
        // cursor stays under the cursor; the pre-drag world deltas
        // are captured once at drag-start so accumulating per-frame
        // rounding can't drift the anchor.
        bool panning_ = false;
        math::Vector<float, 3> panStartCameraPos_{};
        math::Vector<float, 2> panStartNorm_{};
        math::Vector<float, 3> panWorldPerNormX_{};
        math::Vector<float, 3> panWorldPerNormY_{};

        // "Zoom" parameter: vertical extent of the camera's content in
        // world units. The horizontal extent is derived per-frame by
        // multiplying by the panel's aspect ratio, so showAll fits the
        // content into the square RT exactly with the right shape and
        // ImGui::Image's stretch to panel size doesn't distort.
        float contentHeight_ = 60.0F;

        math::Size<std::uint32_t, 2> currentSize_{0U, 0U};
    };
}

#endif // FLUXION_EDITOR_VIEWPORTPANEL_HPP
