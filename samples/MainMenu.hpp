#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class MainMenu: public fluxion::scene::Scene
    {
    public:
        MainMenu();

    private:
        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;

        fluxion::gui::Menu menu;

        fluxion::EventHandler handler;
        fluxion::gui::Button gitHubButton;
        fluxion::gui::Button spritesButton;
        fluxion::gui::Button guiButton;
        fluxion::gui::Button renderTargetButton;
        fluxion::gui::Button animationsButton;
        fluxion::gui::Button inputButton;
        fluxion::gui::Button soundButton;
        fluxion::gui::Button perspectiveButton;
        fluxion::gui::Button imguiButton;
    };
}

#endif // MAINMENU_HPP
