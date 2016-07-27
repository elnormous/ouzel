// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample(Application& app);
    virtual ~GUISample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    
private:
    Application& application;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;

    ouzel::gui::ButtonPtr button;
    ouzel::gui::CheckBoxPtr checkBox;
};
