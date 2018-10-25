// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "ouzel.hpp"

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample();

private:
    bool handleGamepad(const ouzel::GamepadEvent& event);
    bool handleUI(const ouzel::UIEvent& event);
    bool handleKeyboard(const ouzel::KeyboardEvent& event) const;

    ouzel::EventHandler handler;

    ouzel::gui::Button button;
    ouzel::gui::Button fullscreenButton;
    ouzel::gui::CheckBox checkBox;

    ouzel::gui::Label label1;
    ouzel::gui::Label label2;
    ouzel::gui::Label label3;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
