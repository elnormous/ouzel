#ifndef IMGUISAMPLE_HPP
#define IMGUISAMPLE_HPP

#include "events/EventHandler.hpp"
#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    // Native-Fluxion-UI control panel for the ImGui integration. Each button
    // toggles a flag in the shared ImGuiHost state; the per-frame ImGui draw
    // callback reads those flags and renders the corresponding windows. The
    // "Disable ImGui" button flips the Context's master switch, which makes
    // the backend skip the entire ImGui frame on the render thread.
    class ImGuiSample: public fluxion::scene::Scene
    {
    public:
        ImGuiSample();

    private:
        void refreshLabels();

        fluxion::EventHandler handler;

        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::gui::Menu menu;

        fluxion::gui::Button smokeTestButton;
        fluxion::gui::Button textRefButton;
        fluxion::gui::Button demoButton;
        fluxion::gui::Button reflectionButton;
        fluxion::gui::Button enableImGuiButton;
        fluxion::gui::Button backButton;

        fluxion::gui::Label statusLabel;
    };
}

#endif // IMGUISAMPLE_HPP
