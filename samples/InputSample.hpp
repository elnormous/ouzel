// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class InputSample: public ouzel::scene::Scene
{
public:
    InputSample();

private:
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event);
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event);
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    
    ouzel::EventHandler eventHandler;

    ouzel::gui::Button hideButton;
    std::unique_ptr<ouzel::scene::ParticleSystem> flameParticleSystem;
    std::unique_ptr<ouzel::scene::Actor> flame;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::input::Cursor cursor;
};
