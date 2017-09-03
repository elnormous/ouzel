// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class RTSample: public ouzel::scene::Scene
{
public:
    RTSample();

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;

    std::unique_ptr<ouzel::scene::Layer> rtLayer;
    std::unique_ptr<ouzel::scene::Camera> rtCamera;
    std::unique_ptr<ouzel::scene::Camera> camera1;
    std::unique_ptr<ouzel::scene::Camera> camera2;

    std::unique_ptr<ouzel::scene::Sprite> characterSprite;
    std::unique_ptr<ouzel::scene::Actor> rtCharacter;

    std::unique_ptr<ouzel::scene::Sprite> rtSprite;
    std::unique_ptr<ouzel::scene::Actor> rtActor;

    ouzel::EventHandler eventHandler;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
