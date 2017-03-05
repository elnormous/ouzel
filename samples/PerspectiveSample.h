// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class PerspectiveSample: public ouzel::scene::Scene
{
public:
    PerspectiveSample(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);
    bool handleMouse(ouzel::Event::Type type, const ouzel::MouseEvent& event);
    bool handleTouch(ouzel::Event::Type type, const ouzel::TouchEvent& event);
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    
    Samples& samples;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;

    ouzel::scene::Sprite characterSprite;
    ouzel::scene::Node character;

    ouzel::scene::Sprite floorSprite;
    ouzel::scene::Node floor;

    std::shared_ptr<ouzel::audio::Sound> jumpSound;

    std::unique_ptr<ouzel::scene::Animator> rotate;

    ouzel::EventHandler eventHandler;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
