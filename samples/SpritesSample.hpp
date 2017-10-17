// Copyright (C) 2017 Elviss Strazdins
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

    std::unique_ptr<ouzel::scene::Sprite> characterSprite;
    std::unique_ptr<ouzel::scene::Actor> character;
    std::unique_ptr<ouzel::scene::Sprite> fireSprite;
    std::unique_ptr<ouzel::scene::Actor> fireActor;
    std::unique_ptr<ouzel::scene::Sprite> triangleSprite;
    std::unique_ptr<ouzel::scene::Actor> triangleActor;

    std::unique_ptr<ouzel::scene::Animator> move;

    ouzel::EventHandler eventHandler;

    ouzel::gui::Button hideButton;
    ouzel::gui::Button wireframeButton;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
