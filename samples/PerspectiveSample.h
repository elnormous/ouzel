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

    std::shared_ptr<ouzel::scene::Layer> layer;
    std::shared_ptr<ouzel::scene::Camera> camera;

    std::shared_ptr<ouzel::scene::Sprite> characterSprite;
    std::shared_ptr<ouzel::scene::Node> character;

    std::shared_ptr<ouzel::scene::Sprite> floorSprite;
    std::shared_ptr<ouzel::scene::Node> floor;

    std::shared_ptr<ouzel::audio::Sound> jumpSound;

    std::shared_ptr<ouzel::scene::Animator> rotate;

    ouzel::EventHandler eventHandler;

    std::shared_ptr<ouzel::scene::Layer> guiLayer;
    std::shared_ptr<ouzel::scene::Camera> guiCamera;
    std::shared_ptr<ouzel::gui::Menu> menu;
    std::shared_ptr<ouzel::gui::Button> backButton;
};
