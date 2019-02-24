// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_FILTERS_HPP
#define OUZEL_AUDIO_FILTERS_HPP

#include <cfloat>
#include <utility>
#include "audio/Effect.hpp"
#include "math/Vector.hpp"
#include "scene/Component.hpp"

namespace ouzel
{
    namespace audio
    {
        class Delay final: public Effect
        {
        public:
            Delay(Audio& initAudio);
            ~Delay();

            Delay(const Delay&) = delete;
            Delay& operator=(const Delay&) = delete;
            Delay(Delay&&) = delete;
            Delay& operator=(Delay&&) = delete;

            inline float getDelay() const { return delay; }
            inline void setDelay(float newDelay);

            inline void setDelayRandom(const std::pair<float, float>& newDelayRandom) { delayRandom = newDelayRandom; }
            inline const std::pair<float, float>& getDelayRandom() const { return delayRandom; }

        private:
            float delay = 0.0F;
            std::pair<float, float> delayRandom{0.0F, 0.0F};
        };

        class Gain final: public Effect
        {
        public:
            Gain(Audio& initAudio);
            ~Gain();

            Gain(const Gain&) = delete;
            Gain& operator=(const Gain&) = delete;
            Gain(Gain&&) = delete;
            Gain& operator=(Gain&&) = delete;

            inline float getGain() const { return gain; }
            void setGain(float newGain);

            inline void setGainRandom(const std::pair<float, float>& newGainRandom) { gainRandom = newGainRandom; }
            inline const std::pair<float, float>& getGainRandom() const { return gainRandom; }

        private:
            float gain = 0.0F; // dB
            std::pair<float, float> gainRandom{0.0F, 0.0F};
        };

        class Panner final: public Effect, public scene::Component
        {
        public:
            Panner(Audio& initAudio);
            ~Panner();

            Panner(const Panner&) = delete;
            Panner& operator=(const Panner&) = delete;
            Panner(Panner&&) = delete;
            Panner& operator=(Panner&&) = delete;

            inline const Vector3F& getPosition() const { return position; }
            void setPosition(const Vector3F& newPosition);

            inline float getRolloffFactor() const { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor);

            inline float getMinDistance() const { return minDistance; }
            void setMinDistance(float newMinDistance);

            inline float getMaxDistance() const { return maxDistance; }
            void setMaxDistance(float newMaxDistance);

        private:
            void updateTransform() override;

            Vector3F position;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;
        };

        class Pitch final: public Effect
        {
        public:
            Pitch(Audio& initAudio, float initPitch = 1.0F);
            ~Pitch();

            Pitch(const Pitch&) = delete;
            Pitch& operator=(const Pitch&) = delete;
            Pitch(Pitch&&) = delete;
            Pitch& operator=(Pitch&&) = delete;

            inline float getPitch() const { return pitch; }
            void setPitch(float newPitch);

            inline void setPitchRandom(const std::pair<float, float>& newPitchRandom) { pitchRandom = newPitchRandom; }
            inline const std::pair<float, float>& getPitchRandom() const { return pitchRandom; }

        private:
            float pitch = 1.0f;
            std::pair<float, float> pitchRandom{0.0F, 0.0F};
        };

        class Reverb final: public Effect
        {
        public:
            Reverb(Audio& initAudio, float initDelay = 0.1F, float initDecay = 0.5F);
            ~Reverb();

            Reverb(const Reverb&) = delete;
            Reverb& operator=(const Reverb&) = delete;
            Reverb(Reverb&&) = delete;
            Reverb& operator=(Reverb&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_FILTERS_HPP
