// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::gui::ButtonPtr backButton;
    ouzel::gui::ButtonPtr jumpButton;
    ouzel::gui::ButtonPtr ambientButton;
    ouzel::EventHandler eventHandler;

    ouzel::audio::SoundPtr jumpSound;
    ouzel::audio::SoundPtr ambientSound;
};
