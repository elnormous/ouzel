// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class RTSample: public ouzel::scene::Scene
{
public:
    RTSample(Application& app);
    virtual ~RTSample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    
private:
    Application& application;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;
};
