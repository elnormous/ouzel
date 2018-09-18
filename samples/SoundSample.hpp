// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "ouzel.hpp"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample();

private:
    bool handleGamepad(ouzel::Event::Type type, const ouzel::GamepadEvent& event);
    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event);
    bool handleKeyboard(ouzel::Event::Type type, const ouzel::KeyboardEvent& event) const;

    ouzel::audio::Listener listener;
    ouzel::audio::Mixer soundMixer;

    ouzel::audio::Sound test8BitSound;
    ouzel::audio::Sound test24BitSound;
    ouzel::audio::Sound jumpSound;
    ouzel::audio::Sound ambientSound;
    ouzel::audio::Sound music;

    ouzel::scene::Layer guiLayer;
    ouzel::scene::Camera guiCamera;
    ouzel::scene::Actor guiCameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::scene::Actor soundActor;
    ouzel::gui::Button test8BitButton;
    ouzel::gui::Button test24BitButton;
    ouzel::gui::Button jumpButton;
    ouzel::gui::Button ambientButton;
    ouzel::gui::Button musicButton;

    ouzel::EventHandler handler;
};
