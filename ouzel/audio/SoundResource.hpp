// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cfloat>
#include <memory>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class AudioDevice;
        class SoundData;
        class Stream;
        
        class SoundResource: public Noncopyable
        {
            friend Audio;
        public:
            SoundResource(AudioDevice* aAudioDevice);
            virtual ~SoundResource();

            virtual bool init(const std::shared_ptr<SoundData>& newSoundData, bool newRelativePosition = false);

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

            bool getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result);

        protected:
            AudioDevice* audioDevice;

            std::shared_ptr<SoundData> soundData;
            std::unique_ptr<Stream> stream;
            bool relativePosition = false;
            bool playing = false;
            bool repeat = false;
            bool reset = false;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;
            float rolloffFactor = 1.0f;
            float minDistance = 1.0f;
            float maxDistance = FLT_MAX;

            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel
