// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "ouzel.hpp"

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu();

private:
    bool handleUI(const ouzel::UIEvent& event);
    bool handleKeyboard(const ouzel::KeyboardEvent& event);

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

#endif // MAINMENU_HPP
