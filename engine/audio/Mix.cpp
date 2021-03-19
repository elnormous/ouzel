// Ouzel by Elviss Strazdins

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
        for (auto submix : inputSubmixes)
            submix->output = nullptr;

        for (auto voice : inputVoices)
            voice->output = nullptr;

        for (auto effect : effects)
            effect->mix = nullptr;

        for (auto listener : listeners)
            listener->mix = nullptr;

        if (busId) audio.deleteObject(busId);
    }

    void Mix::addEffect(Effect* effect)
    {
        if (std::find(effects.begin(), effects.end(), effect) == effects.end())
        {
            if (effect->mix) effect->mix->removeEffect(effect);
            effect->mix = this;
            effects.push_back(effect);

            audio.addCommand(std::make_unique<mixer::AddProcessorCommand>(busId, effect->getProcessorId()));
        }
    }

    void Mix::removeEffect(Effect* effect)
    {
        if (const auto i = std::find(effects.begin(), effects.end(), effect); i != effects.end())
        {
            effect->mix = nullptr;
            effects.erase(i);

            audio.addCommand(std::make_unique<mixer::RemoveProcessorCommand>(busId, effect->getProcessorId()));
        }
    }

    void Mix::addInput(Submix* submix)
    {
        if (std::find(inputSubmixes.begin(), inputSubmixes.end(), submix) == inputSubmixes.end())
            inputSubmixes.push_back(submix);
    }

    void Mix::removeInput(Submix* submix)
    {
        if (const auto i = std::find(inputSubmixes.begin(), inputSubmixes.end(), submix); i != inputSubmixes.end())
            inputSubmixes.erase(i);
    }

    void Mix::addInput(Voice* voice)
    {
        if (std::find(inputVoices.begin(), inputVoices.end(), voice) == inputVoices.end()) inputVoices.push_back(voice);
    }

    void Mix::removeInput(Voice* voice)
    {
        if (const auto i = std::find(inputVoices.begin(), inputVoices.end(), voice); i != inputVoices.end())
            inputVoices.erase(i);
    }

    void Mix::addListener(Listener* listener)
    {
        if (std::find(listeners.begin(), listeners.end(), listener) == listeners.end())
        {
            if (listener->mix) listener->mix->removeListener(listener);
            listener->mix = this;
            listeners.push_back(listener);
        }
    }

    void Mix::removeListener(Listener* listener)
    {
        if (const auto i = std::find(listeners.begin(), listeners.end(), listener); i != listeners.end())
        {
            listener->mix = nullptr;
            listeners.erase(i);
        }
    }
}
