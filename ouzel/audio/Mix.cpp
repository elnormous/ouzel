// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Mix.hpp"
#include "Audio.hpp"
#include "Effect.hpp"
#include "Listener.hpp"
#include "Submix.hpp"
#include "Voice.hpp"

namespace ouzel
{
    namespace audio
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

            for (Listener* listener : listeners)
                listener->mix = nullptr;

            if (busId) audio.deleteObject(busId);
        }

        void Mix::addInput(Submix* submix)
        {
            auto i = std::find(inputSubmixes.begin(), inputSubmixes.end(), submix);
            if (i == inputSubmixes.end()) inputSubmixes.push_back(submix);
        }

        void Mix::removeInput(Submix* submix)
        {
            auto i = std::find(inputSubmixes.begin(), inputSubmixes.end(), submix);
            if (i != inputSubmixes.end()) inputSubmixes.erase(i);
        }

        void Mix::addInput(Voice* voice)
        {
            auto i = std::find(inputVoices.begin(), inputVoices.end(), voice);
            if (i == inputVoices.end()) inputVoices.push_back(voice);
        }

        void Mix::removeInput(Voice* voice)
        {
            auto i = std::find(inputVoices.begin(), inputVoices.end(), voice);
            if (i != inputVoices.end()) inputVoices.erase(i);
        }

        void Mix::addListener(Listener* listener)
        {
            auto i = std::find(listeners.begin(), listeners.end(), listener);

            if (i == listeners.end())
            {
                if (listener->mix) listener->mix->removeListener(listener);
                listener->mix = this;
                listeners.push_back(listener);
            }
        }

        void Mix::removeListener(Listener* listener)
        {
            auto i = std::find(listeners.begin(), listeners.end(), listener);

            if (i != listeners.end())
            {
                listener->mix = nullptr;
                listeners.erase(i);
            }
        }
    } // namespace audio
} // namespace ouzel
