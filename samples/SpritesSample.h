// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample(Application& app);
    virtual ~SpritesSample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;

private:
    Application& application;
    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;

    ouzel::scene::NodePtr character;
    ouzel::gui::ButtonPtr button;

};
