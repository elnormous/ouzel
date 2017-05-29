// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample();

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::EventHandler eventHandler;

    std::unique_ptr<ouzel::gui::Button> button;
    std::unique_ptr<ouzel::gui::Button> fullscreenButton;
    std::unique_ptr<ouzel::gui::CheckBox> checkBox;

    std::unique_ptr<ouzel::gui::Label> label1;
    std::unique_ptr<ouzel::gui::Label> label2;
    std::unique_ptr<ouzel::gui::Label> label3;

    std::unique_ptr<ouzel::scene::Layer> guiLayer;
    std::unique_ptr<ouzel::scene::Camera> guiCamera;
    std::unique_ptr<ouzel::gui::Menu> menu;
    std::unique_ptr<ouzel::gui::Button> backButton;
};
