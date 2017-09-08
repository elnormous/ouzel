// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cfloat>
#include <memory>
#include "utils/Noncopyable.hpp"
#include "audio/SoundInput.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class SoundData;
        class SoundResource;

        class Sound: public Noncopyable, public SoundInput
        {
            friend Audio;
        public:
            Sound();
            virtual ~Sound();

            bool init(const std::shared_ptr<SoundData>& newSoundData);

            const std::shared_ptr<SoundData>& getSoundData() const { return soundData; }

            const Vector3& getPosition() const { return position; }
            void setPosition(const Vector3& newPosition);

            float getPitch() const { return pitch; }
            void setPitch(float newPitch);

            float getGain() const { return gain; }
            void setGain(float newGain);

            float getRolloffFactor() const { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor);

            float getMinDistance() { return minDistance; }
            void setMinDistance(float newMinDistance);

            float getMaxDistance() { return maxDistance; }
            void setMaxDistance(float newMaxDistance);

            bool play(bool repeatSound = false);
            bool pause();
            bool stop();

            bool isRepeating() const { return repeating; }

            virtual AudioDevice::RenderCommand getRenderCommand() override;

        private:
            SoundResource* resource = nullptr;

            std::shared_ptr<SoundData> soundData;
            bool repeating = false;

            Vector3 position;
            float pitch = 1.0f;
            float gain = 1.0f;
            float rolloffFactor = 1.0f;
            float minDistance = 1.0f;
            float maxDistance = FLT_MAX;
        };
    } // namespace audio
} // namespace ouzel
