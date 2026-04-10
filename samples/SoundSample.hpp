#ifndef SOUNDSAMPLE_HPP
#define SOUNDSAMPLE_HPP

#include "audio/Effects.hpp"
#include "audio/Listener.hpp"
#include "audio/Submix.hpp"
#include "gui/Widgets.hpp"
#include "scene/Camera.hpp"
#include "scene/Layer.hpp"
#include "scene/Scene.hpp"

namespace samples
{
    class SoundSample: public fluxion::scene::Scene
    {
    public:
        SoundSample();

    private:
        fluxion::audio::Submix submix;
        fluxion::audio::Listener listener;
        fluxion::audio::Gain soundGain;

        fluxion::audio::Submix test8BitSubmix;
        fluxion::audio::Voice test8BitVoice;
        fluxion::audio::PitchScale test8BitPitch;

        fluxion::audio::Submix test24BitSubmix;
        fluxion::audio::Voice test24BitVoice;
        fluxion::audio::PitchScale test24BitPitch;

        fluxion::audio::Submix jumpSubmix;
        fluxion::audio::Voice jumpVoice;
        fluxion::audio::Panner jumpPanner;
        fluxion::audio::Reverb jumpReverb;

        fluxion::audio::Voice ambientVoice;
        fluxion::audio::Voice music;
        fluxion::audio::Oscillator tone;
        fluxion::audio::Voice toneVoice;

        fluxion::scene::Layer layer;
        fluxion::scene::Camera camera;
        fluxion::scene::Actor cameraActor;
        fluxion::gui::Menu menu;
        fluxion::gui::Button backButton;

        fluxion::scene::Actor soundActor;
        fluxion::gui::Button test8BitButton;
        fluxion::gui::Button test24BitButton;
        fluxion::gui::Button jumpButton;
        fluxion::gui::Button ambientButton;
        fluxion::gui::Button musicButton;
        fluxion::gui::Button toneButton;

        fluxion::EventHandler handler;
    };
}

#endif // SOUNDSAMPLE_HPP
