// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Samples.h"

class MainMenu: public ouzel::scene::Scene
{
public:
    MainMenu(Samples& aSamples);

private:
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event);

    Samples& samples;

    std::shared_ptr<ouzel::scene::Layer> layer;
    std::shared_ptr<ouzel::scene::Camera> camera;

    std::shared_ptr<ouzel::gui::Menu> menu;

    ouzel::EventHandler eventHandler;
    std::shared_ptr<ouzel::gui::Button> gitHubButton;
    std::shared_ptr<ouzel::gui::Button> spritesButton;
    std::shared_ptr<ouzel::gui::Button> guiButton;
    std::shared_ptr<ouzel::gui::Button> renderTargetButton;
    std::shared_ptr<ouzel::gui::Button> animationsButton;
    std::shared_ptr<ouzel::gui::Button> inputButton;
    std::shared_ptr<ouzel::gui::Button> soundButton;
    std::shared_ptr<ouzel::gui::Button> perspectiveButton;
};
