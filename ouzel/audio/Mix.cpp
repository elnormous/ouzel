// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Mix.hpp"
#include "Audio.hpp"
#include "Filter.hpp"
#include "Listener.hpp"
#include "Sound.hpp"
#include "Submix.hpp"

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

            for (Sound* sound : inputSounds)
                sound->output = nullptr;

            for (Filter* filter : filters)
                filter->mix = nullptr;

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

        void Mix::addInput(Sound* sound)
        {
            auto i = std::find(inputSounds.begin(), inputSounds.end(), sound);
            if (i == inputSounds.end()) inputSounds.push_back(sound);
        }

        void Mix::removeInput(Sound* sound)
        {
            auto i = std::find(inputSounds.begin(), inputSounds.end(), sound);
            if (i != inputSounds.end()) inputSounds.erase(i);
        }

        void Mix::addFilter(Filter* filter)
        {
            auto i = std::find(filters.begin(), filters.end(), filter);
            if (i == filters.end()) filters.push_back(filter);
        }

        void Mix::removeFilter(Filter* filter)
        {
            auto i = std::find(filters.begin(), filters.end(), filter);
            if (i != filters.end()) filters.erase(i);
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
