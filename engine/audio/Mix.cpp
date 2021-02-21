// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "Mix.hpp"
#include "Audio.hpp"
#include "Effect.hpp"
#include "Listener.hpp"
#include "Submix.hpp"
#include "Voice.hpp"

namespace ouzel::audio
{
    Mix::Mix(Audio& initAudio):
        audio(initAudio),
        busId(audio.initBus())
    {
    }

    Mix::~Mix()
    {
        for (Submix* submix : inputSubmixes)
            submix->output = nullptr;

        for (Voice* voice : inputVoices)
            voice->output = nullptr;

        for (Effect* effect : effects)
            effect->mix = nullptr;

        for (Listener* listener : listeners)
            listener->mix = nullptr;

        if (busId) audio.deleteObject(busId);
    }

    void Mix::addEffect(Effect* effect)
    {
        const auto i = std::find(effects.begin(), effects.end(), effect);
        if (i == effects.end())
        {
            if (effect->mix) effect->mix->removeEffect(effect);
            effect->mix = this;
            effects.push_back(effect);

            audio.addCommand(std::make_unique<mixer::AddProcessorCommand>(busId, effect->getProcessorId()));
        }
    }

    void Mix::removeEffect(Effect* effect)
    {
        const auto i = std::find(effects.begin(), effects.end(), effect);
        if (i != effects.end())
        {
            effect->mix = nullptr;
            effects.erase(i);

            audio.addCommand(std::make_unique<mixer::RemoveProcessorCommand>(busId, effect->getProcessorId()));
        }
    }

    void Mix::addInput(Submix* submix)
    {
        const auto i = std::find(inputSubmixes.begin(), inputSubmixes.end(), submix);
        if (i == inputSubmixes.end()) inputSubmixes.push_back(submix);
    }

    void Mix::removeInput(Submix* submix)
    {
        const auto i = std::find(inputSubmixes.begin(), inputSubmixes.end(), submix);
        if (i != inputSubmixes.end()) inputSubmixes.erase(i);
    }

    void Mix::addInput(Voice* voice)
    {
        const auto i = std::find(inputVoices.begin(), inputVoices.end(), voice);
        if (i == inputVoices.end()) inputVoices.push_back(voice);
    }

    void Mix::removeInput(Voice* voice)
    {
        const auto i = std::find(inputVoices.begin(), inputVoices.end(), voice);
        if (i != inputVoices.end()) inputVoices.erase(i);
    }

    void Mix::addListener(Listener* listener)
    {
        const auto i = std::find(listeners.begin(), listeners.end(), listener);
        if (i == listeners.end())
        {
            if (listener->mix) listener->mix->removeListener(listener);
            listener->mix = this;
            listeners.push_back(listener);
        }
    }

    void Mix::removeListener(Listener* listener)
    {
        const auto i = std::find(listeners.begin(), listeners.end(), listener);
        if (i != listeners.end())
        {
            listener->mix = nullptr;
            listeners.erase(i);
        }
    }
}
