// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class SpritesSample: public ouzel::scene::Scene
{
public:
    SpritesSample();
    virtual ~SpritesSample();

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::scene::LayerPtr layer;

    ouzel::gui::ButtonPtr backButton;
    ouzel::EventHandler eventHandler;

    ouzel::scene::NodePtr character;
    ouzel::gui::ButtonPtr hideButton;
    ouzel::gui::ButtonPtr wireframeButton;
};
