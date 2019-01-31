// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifndef GUISAMPLE_HPP
#define GUISAMPLE_HPP

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

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};

#endif // GUISAMPLE_HPP
