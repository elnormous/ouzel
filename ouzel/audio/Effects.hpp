// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EFFECTS_HPP
#define OUZEL_AUDIO_EFFECTS_HPP

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
            Delay(Audio& initAudio, float initDelay = 0.0F);
            ~Delay();

            Delay(const Delay&) = delete;
            Delay& operator=(const Delay&) = delete;
            Delay(Delay&&) = delete;
            Delay& operator=(Delay&&) = delete;

            inline auto getDelay() const noexcept { return delay; }
            void setDelay(float newDelay);

            inline const std::pair<float, float>& getDelayRandom() const noexcept { return delayRandom; }
            void setDelayRandom(const std::pair<float, float>& newDelayRandom);

        private:
            float delay = 0.0F;
            std::pair<float, float> delayRandom{0.0F, 0.0F};
        };

        class Gain final: public Effect
        {
        public:
            Gain(Audio& initAudio, float initGain = 0.0F);
            ~Gain();

            Gain(const Gain&) = delete;
            Gain& operator=(const Gain&) = delete;
            Gain(Gain&&) = delete;
            Gain& operator=(Gain&&) = delete;

            inline auto getGain() const noexcept { return gain; }
            void setGain(float newGain);

            inline const std::pair<float, float>& getGainRandom() const noexcept { return gainRandom; }
            void setGainRandom(const std::pair<float, float>& newGainRandom);

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

            inline auto& getPosition() const noexcept { return position; }
            void setPosition(const Vector3F& newPosition);

            inline auto getRolloffFactor() const noexcept { return rolloffFactor; }
            void setRolloffFactor(float newRolloffFactor);

            inline auto getMinDistance() const noexcept { return minDistance; }
            void setMinDistance(float newMinDistance);

            inline auto getMaxDistance() const noexcept { return maxDistance; }
            void setMaxDistance(float newMaxDistance);

        private:
            void updateTransform() final;

            Vector3F position;
            float rolloffFactor = 1.0F;
            float minDistance = 1.0F;
            float maxDistance = FLT_MAX;
        };

        class PitchScale final: public Effect
        {
        public:
            PitchScale(Audio& initAudio, float initScale = 1.0F);
            ~PitchScale();

            PitchScale(const PitchScale&) = delete;
            PitchScale& operator=(const PitchScale&) = delete;
            PitchScale(PitchScale&&) = delete;
            PitchScale& operator=(PitchScale&&) = delete;

            inline auto getScale() const noexcept { return scale; }
            void setScale(float newScale);

            inline const std::pair<float, float>& getScaleRandom() const noexcept { return scaleRandom; }
            void setScaleRandom(const std::pair<float, float>& newScaleRandom);

        private:
            float scale = 1.0F;
            std::pair<float, float> scaleRandom{0.0F, 0.0F};
        };

        class PitchShift final: public Effect
        {
        public:
            PitchShift(Audio& initAudio, float initShift = 1.0F);
            ~PitchShift();

            PitchShift(const PitchShift&) = delete;
            PitchShift& operator=(const PitchShift&) = delete;
            PitchShift(PitchShift&&) = delete;
            PitchShift& operator=(PitchShift&&) = delete;

            inline auto getShift() const noexcept { return shift; }
            void setShift(float newShift);

            inline const std::pair<float, float>& getShiftRandom() const noexcept { return shiftRandom; }
            void setShiftRandom(const std::pair<float, float>& newShiftRandom);

        private:
            float shift = 1.0f;
            std::pair<float, float> shiftRandom{0.0F, 0.0F};
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

            inline auto getDelay() const noexcept { return delay; }
            inline auto getDecay() const noexcept { return decay; }

        private:
            float delay = 0.1F;
            float decay = 0.5F;
        };

        class LowPass final: public Effect
        {
        public:
            LowPass(Audio& initAudio);
            ~LowPass();

            LowPass(const LowPass&) = delete;
            LowPass& operator=(const LowPass&) = delete;
            LowPass(LowPass&&) = delete;
            LowPass& operator=(LowPass&&) = delete;
        };

        class HighPass final: public Effect
        {
        public:
            HighPass(Audio& initAudio);
            ~HighPass();

            HighPass(const HighPass&) = delete;
            HighPass& operator=(const HighPass&) = delete;
            HighPass(HighPass&&) = delete;
            HighPass& operator=(HighPass&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EFFECTS_HPP
