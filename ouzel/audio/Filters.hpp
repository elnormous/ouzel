// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_FILTERS_HPP
#define OUZEL_AUDIO_FILTERS_HPP

#include <cfloat>
#include "audio/Filter.hpp"
#include "math/Vector3.hpp"
#include "scene/Component.hpp"

namespace ouzel
{
    namespace audio
    {
        class Delay final: public Filter
        {
        public:
            Delay(Audio& initAudio);
            ~Delay();

            Delay(const Delay&) = delete;
            Delay& operator=(const Delay&) = delete;
            Delay(Delay&&) = delete;
            Delay& operator=(Delay&&) = delete;

            float getDelay() const { return delay; }
            void setDelay(float newDelay);

        private:
            float delay = 0.0F;
        };

        class Gain final: public Filter
        {
        public:
            Gain(Audio& initAudio);
            ~Gain();

            Gain(const Gain&) = delete;
            Gain& operator=(const Gain&) = delete;
            Gain(Gain&&) = delete;
            Gain& operator=(Gain&&) = delete;

            float getGain() const { return gain; }
            void setGain(float newGain);

        private:
            float gain = 0.0F; // dB
        };

        class Panner final: public Filter, public scene::Component
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

            Vector3 position;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;
        };

        class Pitch final: public Filter
        {
        public:
            Pitch(Audio& initAudio, float initPitch = 1.0F);
            ~Pitch();

            Pitch(const Pitch&) = delete;
            Pitch& operator=(const Pitch&) = delete;
            Pitch(Pitch&&) = delete;
            Pitch& operator=(Pitch&&) = delete;

            float getPitch() const { return pitch; }
            void setPitch(float newPitch);

        private:
            float pitch = 1.0f;
        };

        class Reverb final: public Filter
        {
        public:
            Reverb(Audio& initAudio);
            ~Reverb();

            Reverb(const Reverb&) = delete;
            Reverb& operator=(const Reverb&) = delete;
            Reverb(Reverb&&) = delete;
            Reverb& operator=(Reverb&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_FILTERS_HPP
