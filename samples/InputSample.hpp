// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifndef INPUTSAMPLE_HPP
#define INPUTSAMPLE_HPP

#include "ouzel.hpp"

class InputSample: public ouzel::scene::Scene
{
public:
    InputSample();

private:
    bool handleKeyboard(const ouzel::KeyboardEvent& event);
    bool handleMouse(const ouzel::MouseEvent& event);
    bool handleTouch(const ouzel::TouchEvent& event);
    bool handleGamepad(const ouzel::GamepadEvent& event);
    bool handleUI(const ouzel::UIEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::EventHandler handler;

    ouzel::gui::Button hideButton;
    ouzel::gui::Button discoverButton;
    ouzel::scene::ParticleSystem flameParticleSystem;
    ouzel::scene::Actor flame;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::input::Cursor cursor;
};

#endif // INPUTSAMPLE_HPP
