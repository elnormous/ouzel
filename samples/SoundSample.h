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

    std::unique_ptr<ouzel::audio::Sound> test8BitSound;
    std::unique_ptr<ouzel::audio::Sound> test24BitSound;
    std::unique_ptr<ouzel::audio::Sound> jumpSound;
    std::unique_ptr<ouzel::audio::Sound> ambientSound;

    std::unique_ptr<ouzel::scene::Layer> guiLayer;
    std::unique_ptr<ouzel::scene::Camera> guiCamera;
    std::unique_ptr<ouzel::gui::Menu> menu;
    std::unique_ptr<ouzel::gui::Button> backButton;

    std::unique_ptr<ouzel::gui::Button> test8BitButton;
    std::unique_ptr<ouzel::gui::Button> test24BitButton;
    std::unique_ptr<ouzel::gui::Button> jumpButton;
    std::unique_ptr<ouzel::gui::Button> ambientButton;

    ouzel::EventHandler eventHandler;
};
