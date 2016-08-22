// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample(Samples& pSamples);
    virtual ~GUISample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

private:
    Samples& samples;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;

    ouzel::gui::ButtonPtr button;
    ouzel::gui::ButtonPtr fullscreenButton;
    ouzel::gui::CheckBoxPtr checkBox;
};
