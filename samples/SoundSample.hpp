// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample();

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event) const;
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::audio::Listener listener;
    ouzel::audio::Mixer soundMixer;

    std::unique_ptr<ouzel::audio::Sound> test8BitSound;
    std::unique_ptr<ouzel::audio::Sound> test24BitSound;
    std::unique_ptr<ouzel::audio::Sound> jumpSound;
    std::unique_ptr<ouzel::audio::Sound> ambientSound;
    std::unique_ptr<ouzel::audio::Sound> music;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::gui::Button test8BitButton;
    ouzel::gui::Button test24BitButton;
    ouzel::gui::Button jumpButton;
    ouzel::gui::Button ambientButton;
    ouzel::gui::Button musicButton;

    ouzel::EventHandler eventHandler;
};
