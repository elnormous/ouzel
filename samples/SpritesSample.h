// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample(Samples& aSamples);

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    std::unique_ptr<ouzel::scene::Layer> layer;
    std::unique_ptr<ouzel::scene::Camera> camera;

    std::unique_ptr<ouzel::scene::Sprite> characterSprite;
    std::unique_ptr<ouzel::scene::Node> character;
    std::unique_ptr<ouzel::scene::Sprite> fireSprite;
    std::unique_ptr<ouzel::scene::Node> fireNode;
    std::unique_ptr<ouzel::scene::Sprite> triangleSprite;
    std::unique_ptr<ouzel::scene::Node> triangleNode;

    std::unique_ptr<ouzel::scene::Animator> move;

    ouzel::EventHandler eventHandler;

    std::unique_ptr<ouzel::gui::Button> hideButton;
    std::unique_ptr<ouzel::gui::Button> wireframeButton;

    std::unique_ptr<ouzel::scene::Layer> guiLayer;
    std::unique_ptr<ouzel::scene::Camera> guiCamera;
    std::unique_ptr<ouzel::gui::Menu> menu;
    std::unique_ptr<ouzel::gui::Button> backButton;
};
