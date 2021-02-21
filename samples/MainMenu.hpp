// Ouzel by Elviss Strazdins

#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class MainMenu: public ouzel::scene::Scene
    {
    public:
        MainMenu();

    private:
        ouzel::scene::Layer layer;
        ouzel::scene::Camera camera;
        ouzel::scene::Actor cameraActor;

        ouzel::gui::Menu menu;

        ouzel::EventHandler handler;
        ouzel::gui::Button gitHubButton;
        ouzel::gui::Button spritesButton;
        ouzel::gui::Button guiButton;
        ouzel::gui::Button renderTargetButton;
        ouzel::gui::Button animationsButton;
        ouzel::gui::Button inputButton;
        ouzel::gui::Button soundButton;
        ouzel::gui::Button perspectiveButton;
    };
}

#endif // MAINMENU_HPP
