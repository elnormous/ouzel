// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample();

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;

    ouzel::scene::Sprite characterSprite;
    ouzel::scene::Actor character;
    ouzel::scene::Sprite fireSprite;
    ouzel::scene::Actor fireActor;
    ouzel::scene::Sprite triangleSprite;
    ouzel::scene::Actor triangleActor;

    std::unique_ptr<ouzel::scene::Animator> move;

    ouzel::EventHandler handler;

    ouzel::gui::Button hideButton;
    ouzel::gui::Button wireframeButton;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
