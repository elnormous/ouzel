// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class InputSample: public ouzel::scene::Scene
{
public:
    InputSample(Application& app);
    virtual ~InputSample();

    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event, const ouzel::VoidPtr& sender) const;
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event, const ouzel::VoidPtr& sender) const;

private:
    Application& application;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;
    
    ouzel::gui::ButtonPtr button;
    ouzel::scene::NodePtr flame;
    ouzel::scene::CameraPtr camera;
};
