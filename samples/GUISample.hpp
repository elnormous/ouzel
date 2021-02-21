// Ouzel by Elviss Strazdins

#ifndef GUISAMPLE_HPP
#define GUISAMPLE_HPP

#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class GUISample: public ouzel::scene::Scene
    {
    public:
        GUISample();

    private:
        ouzel::EventHandler handler;

        ouzel::gui::Button button;
        ouzel::gui::Button fullscreenButton;
        ouzel::gui::Button minimizeButton;
        ouzel::gui::Button maximizeButton;
        ouzel::gui::CheckBox checkBox;

        ouzel::gui::Label label1;
        ouzel::gui::Label label2;
        ouzel::gui::Label label3;

        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;
        ouzel::gui::Menu menu;
        ouzel::gui::Button backButton;
    };
}

#endif // GUISAMPLE_HPP
