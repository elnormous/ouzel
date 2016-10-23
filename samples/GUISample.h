// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    ouzel::EventHandler eventHandler;

    std::unique_ptr<ouzel::gui::Button> button;
    std::unique_ptr<ouzel::gui::Button> fullscreenButton;
    std::unique_ptr<ouzel::gui::CheckBox> checkBox;

    std::unique_ptr<ouzel::gui::Label> label1;
    std::unique_ptr<ouzel::gui::Label> label2;
    std::unique_ptr<ouzel::gui::Label> label3;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;
};
