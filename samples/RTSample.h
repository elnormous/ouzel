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

    std::shared_ptr<ouzel::scene::Layer> layer;
    std::shared_ptr<ouzel::scene::Camera> camera;

    std::shared_ptr<ouzel::scene::Layer> rtLayer;
    std::shared_ptr<ouzel::scene::Camera> rtCamera;
    std::shared_ptr<ouzel::scene::Camera> camera1;
    std::shared_ptr<ouzel::scene::Camera> camera2;

    std::shared_ptr<ouzel::scene::Sprite> characterSprite;
    std::shared_ptr<ouzel::scene::Node> rtCharacter;

    std::shared_ptr<ouzel::scene::Sprite> rtSprite;
    std::shared_ptr<ouzel::scene::Node> rtNode;

    ouzel::EventHandler eventHandler;

    std::shared_ptr<ouzel::scene::Layer> guiLayer;
    std::shared_ptr<ouzel::scene::Camera> guiCamera;
    std::shared_ptr<ouzel::gui::Menu> menu;
    std::shared_ptr<ouzel::gui::Button> backButton;
};
