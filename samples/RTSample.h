// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class RTSample: public ouzel::scene::Scene
{
public:
    RTSample(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;

    ouzel::scene::Layer rtLayer;
    ouzel::scene::Camera rtCamera;
    ouzel::scene::Camera camera1;
    ouzel::scene::Camera camera2;

    ouzel::scene::Sprite characterSprite;
    ouzel::scene::Node rtCharacter;

    ouzel::scene::Sprite rtSprite;
    ouzel::scene::Node rtNode;

    ouzel::EventHandler eventHandler;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
