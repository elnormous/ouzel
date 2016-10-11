// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class InputSample: public ouzel::scene::Scene
{
public:
    InputSample();
    virtual ~InputSample();

private:
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event) const;
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event) const;
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event) const;
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;

    ouzel::gui::ButtonPtr button;
    ouzel::scene::NodePtr flame;
    ouzel::scene::CameraPtr camera;
};
