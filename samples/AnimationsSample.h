// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class AnimationsSample: public ouzel::scene::Scene
{
public:
    AnimationsSample(Samples& pSamples);
    virtual ~AnimationsSample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

private:
    Samples& samples;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;
};
