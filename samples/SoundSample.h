// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample(Samples& aSamples);

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;
    
    ouzel::gui::Button jumpButton;
    ouzel::gui::Button ambientButton;
    ouzel::EventHandler eventHandler;

    ouzel::audio::SoundPtr jumpSound;
    ouzel::audio::SoundPtr ambientSound;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
