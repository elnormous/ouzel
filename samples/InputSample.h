// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class InputSample: public ouzel::scene::Scene
{
public:
    InputSample(Samples& aSamples);

private:
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event);
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event);
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

    Samples& samples;

    std::shared_ptr<ouzel::scene::Layer> layer;
    std::shared_ptr<ouzel::scene::Camera> camera;
    
    ouzel::EventHandler eventHandler;

    std::shared_ptr<ouzel::gui::Button> button;
    std::shared_ptr<ouzel::scene::ParticleSystem> flameParticleSystem;
    std::shared_ptr<ouzel::scene::Node> flame;

    std::shared_ptr<ouzel::scene::Layer> guiLayer;
    std::shared_ptr<ouzel::scene::Camera> guiCamera;
    std::shared_ptr<ouzel::gui::Menu> menu;
    std::shared_ptr<ouzel::gui::Button> backButton;
};
