// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);

    Samples& samples;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;

    ouzel::gui::Menu menu;

    ouzel::EventHandler eventHandler;
    ouzel::gui::Button spritesButton;
    ouzel::gui::Button guiButton;
    ouzel::gui::Button renderTargetButton;
    ouzel::gui::Button animationsButton;
    ouzel::gui::Button inputButton;
    ouzel::gui::Button soundButton;
    ouzel::gui::Button perspectiveButton;
};
