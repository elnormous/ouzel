// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class RTSample: public ouzel::scene::Scene
{
public:
    RTSample(Samples& aSamples);

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    std::unique_ptr<ouzel::scene::Layer> layer;
    std::unique_ptr<ouzel::scene::Camera> camera;

    std::unique_ptr<ouzel::scene::Layer> rtLayer;
    std::unique_ptr<ouzel::scene::Camera> rtCamera;
    std::unique_ptr<ouzel::scene::Camera> camera1;
    std::unique_ptr<ouzel::scene::Camera> camera2;

    std::unique_ptr<ouzel::scene::Sprite> characterSprite;
    std::unique_ptr<ouzel::scene::Node> rtCharacter;

    std::unique_ptr<ouzel::scene::Sprite> rtSprite;
    std::unique_ptr<ouzel::scene::Node> rtNode;

    ouzel::EventHandler eventHandler;

    std::unique_ptr<ouzel::scene::Layer> guiLayer;
    std::unique_ptr<ouzel::scene::Camera> guiCamera;
    std::unique_ptr<ouzel::gui::Menu> menu;
    std::unique_ptr<ouzel::gui::Button> backButton;
};
