// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu(Application& app);
    virtual ~MainMenu();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event, const ouzel::VoidPtr& sender);

private:
    Application& application;
    ouzel::EventHandlerPtr eventHandler;
    ouzel::gui::ButtonPtr spritesButton;
    ouzel::gui::ButtonPtr GUIButton;
    ouzel::gui::ButtonPtr renderTargetButton;
    ouzel::gui::ButtonPtr animationsButton;
    ouzel::gui::ButtonPtr inputButton;
};
