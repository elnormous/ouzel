// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cfloat>
#include <memory>
#include <vector>
#include "audio/Resource.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class AudioDevice;
        class Output;
        class SoundData;
        class Stream;
        
        class SoundResource: public Resource
        {
            friend Audio;
        public:
            SoundResource(AudioDevice* aAudioDevice);
            virtual ~SoundResource();

            bool init(const std::shared_ptr<SoundData>& newSoundData, bool newRelativePosition = false);

            void setPosition(const Vector3& newPosition);
            void setPitch(float newPitch);
            void setGain(float newGain);
            void setRolloffFactor(float newRolloffFactor);
            void setMinDistance(float newMinDistance);
            void setMaxDistance(float newMaxDistance);

            bool play(bool repeatSound = false);
            bool pause();
            bool stop();

            bool isRepeating() const { return repeat; }

            bool isRelativePosition() const { return relativePosition; }
            void setRelativePosition(bool newRelativePosition) { relativePosition = newRelativePosition; }

            virtual bool getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result);

        protected:
            AudioDevice* audioDevice;

            std::shared_ptr<SoundData> soundData;
            std::unique_ptr<Stream> stream;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;
            float rolloffFactor = 1.0f;
            float minDistance = 1.0f;
            float maxDistance = FLT_MAX;

            bool relativePosition = false;
            bool playing = false;
            bool repeat = false;
            bool reset = false;

            std::vector<float> data;
            std::vector<float> resampledData;
        };
    } // namespace audio
} // namespace ouzel
