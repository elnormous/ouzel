// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample(Application& app);
    virtual ~SoundSample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

private:
    Application& application;
    ouzel::gui::ButtonPtr backButton;
    ouzel::gui::ButtonPtr jumpButton;
    ouzel::gui::ButtonPtr ambientButton;
    ouzel::EventHandler eventHandler;

    ouzel::audio::SoundPtr jumpSound;
    ouzel::audio::SoundPtr ambientSound;
};
