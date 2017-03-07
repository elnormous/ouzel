// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class AnimationsSample: public ouzel::scene::Scene
{
public:
    AnimationsSample(Samples& aSamples);

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    std::shared_ptr<ouzel::scene::Layer> layer;
    std::shared_ptr<ouzel::scene::Camera> camera;

    std::shared_ptr<ouzel::scene::ShapeDrawable> shapeDrawable;
    std::shared_ptr<ouzel::scene::Node> drawNode;

    std::shared_ptr<ouzel::scene::Shake> shake;

    std::shared_ptr<ouzel::scene::Sprite> witchSprite;
    std::shared_ptr<ouzel::scene::Node> witch;

    std::shared_ptr<ouzel::scene::Animator> witchScale;
    std::shared_ptr<ouzel::scene::Animator> witchFade;
    std::shared_ptr<ouzel::scene::Animator> witchRotate;
    std::shared_ptr<ouzel::scene::Animator> witchRepeat;
    std::shared_ptr<ouzel::scene::Animator> witchParallel;
    std::shared_ptr<ouzel::scene::Animator> witchSequence;

    std::shared_ptr<ouzel::scene::Sprite> ballSprite;
    std::shared_ptr<ouzel::scene::Node> ball;

    std::shared_ptr<ouzel::scene::Animator> ballDelay;
    std::shared_ptr<ouzel::scene::Animator> ballEase;
    std::shared_ptr<ouzel::scene::Animator> ballMove;
    std::shared_ptr<ouzel::scene::Animator> ballSequence;
    
    ouzel::EventHandler eventHandler;

    std::shared_ptr<ouzel::scene::Layer> guiLayer;
    std::shared_ptr<ouzel::scene::Camera> guiCamera;
    std::shared_ptr<ouzel::gui::Menu> menu;
    std::shared_ptr<ouzel::gui::Button> backButton;
};
