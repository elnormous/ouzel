// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef SOUNDSAMPLE_HPP
#define SOUNDSAMPLE_HPP

#include "ouzel.hpp"

class SoundSample: public ouzel::scene::Scene
{
public:
    SoundSample();

private:
    ouzel::audio::Submix submix;
    ouzel::audio::Listener listener;
    ouzel::audio::Gain soundGain;

    ouzel::audio::Voice test8Bit;
    ouzel::audio::Voice test24Bit;
    ouzel::audio::Voice jump;
    ouzel::audio::Voice ambient;
    ouzel::audio::Voice music;
    ouzel::audio::Voice tone;

    ouzel::scene::Layer layer;
    ouzel::scene::Camera camera;
    ouzel::scene::Actor cameraActor;
    ouzel::gui::Menu menu;
    ouzel::gui::Button backButton;

    ouzel::scene::Actor soundActor;
    ouzel::gui::Button test8BitButton;
    ouzel::gui::Button test24BitButton;
    ouzel::gui::Button jumpButton;
    ouzel::gui::Button ambientButton;
    ouzel::gui::Button musicButton;
    ouzel::gui::Button toneButton;

    ouzel::EventHandler handler;
};

#endif // SOUNDSAMPLE_HPP
