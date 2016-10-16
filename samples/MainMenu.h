// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);
    
    ouzel::EventHandler eventHandler;
    ouzel::gui::ButtonPtr spritesButton;
    ouzel::gui::ButtonPtr GUIButton;
    ouzel::gui::ButtonPtr renderTargetButton;
    ouzel::gui::ButtonPtr animationsButton;
    ouzel::gui::ButtonPtr inputButton;
    ouzel::gui::ButtonPtr soundButton;
};
