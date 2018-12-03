// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifndef SOUNDSAMPLE_HPP
#define SOUNDSAMPLE_HPP

#include "ouzel.hpp"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample();

private:
    bool handleGamepad(const ouzel::GamepadEvent& event);
    bool handleUI(const ouzel::UIEvent& event);
    bool handleKeyboard(const ouzel::KeyboardEvent& event) const;

    ouzel::audio::Submix submix;
    ouzel::audio::Listener listener;
    ouzel::audio::Gain soundGain;

    ouzel::audio::Submix test8BitSubmix;
    ouzel::audio::Sound test8BitSound;
    ouzel::audio::Pitch test8BitPitch;

    ouzel::audio::Submix test24BitSubmix;
    ouzel::audio::Sound test24BitSound;
    ouzel::audio::Pitch test24BitPitch;

    ouzel::audio::Submix jumpSubmix;
    ouzel::audio::Sound jumpSound;
    ouzel::audio::Panner jumpPanner;

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

#endif // SOUNDSAMPLE_HPP
