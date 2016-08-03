// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample(Samples& pSamples);
    virtual ~SpritesSample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

private:
    Samples& samples;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;

    ouzel::scene::NodePtr character;
    ouzel::gui::ButtonPtr button;

};
