// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;

    ouzel::scene::Sprite characterSprite;
    ouzel::scene::Node character;
    ouzel::scene::Sprite fireSprite;
    ouzel::scene::Node fireNode;
    ouzel::scene::Sprite triangleSprite;
    ouzel::scene::Node triangleNode;

    std::unique_ptr<ouzel::scene::Animator> move;

    ouzel::EventHandler eventHandler;

    std::unique_ptr<ouzel::gui::Button> hideButton;
    std::unique_ptr<ouzel::gui::Button> wireframeButton;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
