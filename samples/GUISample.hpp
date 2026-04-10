#ifndef GUISAMPLE_HPP
#define GUISAMPLE_HPP

#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class GUISample: public fluxion::scene::Scene
    {
    public:
        GUISample();

    private:
        fluxion::EventHandler handler;

        fluxion::gui::Button button;
        fluxion::gui::Button fullscreenButton;
        fluxion::gui::Button minimizeButton;
        fluxion::gui::Button maximizeButton;
        fluxion::gui::CheckBox checkBox;

        fluxion::gui::Label label1;
        fluxion::gui::Label label2;
        fluxion::gui::Label label3;

        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;
    };
}

#endif // GUISAMPLE_HPP
