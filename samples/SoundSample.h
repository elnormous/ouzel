// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;
    
    std::unique_ptr<ouzel::gui::Button> jumpButton;
    std::unique_ptr<ouzel::gui::Button> ambientButton;
    ouzel::EventHandler eventHandler;

    ouzel::audio::SoundPtr jumpSound;
    ouzel::audio::SoundPtr ambientSound;

    std::unique_ptr<ouzel::scene::Layer> guiLayer;
    std::unique_ptr<ouzel::scene::Camera> guiCamera;
    std::unique_ptr<ouzel::gui::Menu> menu;
    std::unique_ptr<ouzel::gui::Button> backButton;
};
