// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample(Samples& aSamples);

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    Samples& samples;

    ouzel::EventHandler eventHandler;

    std::shared_ptr<ouzel::gui::Button> button;
    std::shared_ptr<ouzel::gui::Button> fullscreenButton;
    std::shared_ptr<ouzel::gui::CheckBox> checkBox;

    std::shared_ptr<ouzel::gui::Label> label1;
    std::shared_ptr<ouzel::gui::Label> label2;
    std::shared_ptr<ouzel::gui::Label> label3;

    std::shared_ptr<ouzel::scene::Layer> guiLayer;
    std::shared_ptr<ouzel::scene::Camera> guiCamera;
    std::shared_ptr<ouzel::gui::Menu> menu;
    std::shared_ptr<ouzel::gui::Button> backButton;
};
