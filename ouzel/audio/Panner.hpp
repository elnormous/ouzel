// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PANNER_HPP
#define OUZEL_AUDIO_PANNER_HPP

#include <cfloat>
#include "audio/SoundInput.hpp"
#include "audio/SoundOutput.hpp"
#include "math/Vector3.hpp"
#include "scene/Component.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Panner final: public SoundInput, public SoundOutput, public scene::Component
        {
        public:
            Panner(Audio& initAudio);
            ~Panner();

            Panner(const Panner&) = delete;
            Panner& operator=(const Panner&) = delete;

            Panner(Panner&&) = delete;
            Panner& operator=(Panner&&) = delete;

            const Vector3& getPosition() const { return position; }
            void setPosition(const Vector3& newPosition);

            inline float getRolloffFactor() const { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor);

            inline float getMinDistance() const { return minDistance; }
            void setMinDistance(float newMinDistance);

            inline float getMaxDistance() const { return maxDistance; }
            void setMaxDistance(float newMaxDistance);

        private:
            void updateTransform() override;

            Audio& audio;
            uintptr_t nodeId = 0;
            Vector3 position;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PANNER_HPP
