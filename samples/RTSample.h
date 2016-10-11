// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class RTSample: public ouzel::scene::Scene
{
public:
    RTSample();
    virtual ~RTSample();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;
};
