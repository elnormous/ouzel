// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);

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
