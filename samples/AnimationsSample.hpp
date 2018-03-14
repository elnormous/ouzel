// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class AnimationsSample: public ouzel::scene::Scene
{
public:
    AnimationsSample();

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::scene::ShapeRenderer shapeDrawable;
    ouzel::scene::Actor drawActor;

    ouzel::UniquePtr<ouzel::scene::Shake> shake;

    ouzel::scene::Sprite witchSprite;
    ouzel::scene::Actor witch;

    ouzel::UniquePtr<ouzel::scene::Animator> witchScale;
    ouzel::UniquePtr<ouzel::scene::Animator> witchFade;
    ouzel::UniquePtr<ouzel::scene::Animator> witchRotate;
    ouzel::UniquePtr<ouzel::scene::Animator> witchRepeat;
    ouzel::UniquePtr<ouzel::scene::Animator> witchParallel;
    ouzel::UniquePtr<ouzel::scene::Animator> witchSequence;

    ouzel::scene::Sprite ballSprite;
    ouzel::scene::Actor ball;

    ouzel::UniquePtr<ouzel::scene::Animator> ballDelay;
    ouzel::UniquePtr<ouzel::scene::Animator> ballEase;
    ouzel::UniquePtr<ouzel::scene::Animator> ballMove;
    ouzel::UniquePtr<ouzel::scene::Animator> ballSequence;

    ouzel::EventHandler eventHandler;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
