// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EFFECTS_HPP
#define OUZEL_AUDIO_EFFECTS_HPP

#include <cfloat>
#include <utility>
#include "Effect.hpp"
#include "../math/Vector.hpp"
#include "../scene/Component.hpp"

namespace ouzel::audio
{
    class Delay final: public Effect
    {
    public:
        Delay(Audio& initAudio, float initDelay = 0.0F);

        Delay(const Delay&) = delete;
        Delay& operator=(const Delay&) = delete;
        Delay(Delay&&) = delete;
        Delay& operator=(Delay&&) = delete;

        auto getDelay() const noexcept { return delay; }
        void setDelay(float newDelay);

        auto& getDelayRandom() const noexcept { return delayRandom; }
        void setDelayRandom(const std::pair<float, float>& newDelayRandom);

    private:
        float delay = 0.0F;
        std::pair<float, float> delayRandom{0.0F, 0.0F};
    };

    class Gain final: public Effect
    {
    public:
        Gain(Audio& initAudio, float initGain = 0.0F);

        Gain(const Gain&) = delete;
        Gain& operator=(const Gain&) = delete;
        Gain(Gain&&) = delete;
        Gain& operator=(Gain&&) = delete;

        auto getGain() const noexcept { return gain; }
        void setGain(float newGain);

        auto& getGainRandom() const noexcept { return gainRandom; }
        void setGainRandom(const std::pair<float, float>& newGainRandom);

    private:
        float gain = 0.0F; // dB
        std::pair<float, float> gainRandom{0.0F, 0.0F};
    };

    class Panner final: public Effect, public scene::Component
    {
    public:
        explicit Panner(Audio& initAudio);

        Panner(const Panner&) = delete;
        Panner& operator=(const Panner&) = delete;
        Panner(Panner&&) = delete;
        Panner& operator=(Panner&&) = delete;

        auto& getPosition() const noexcept { return position; }
        void setPosition(const Vector3F& newPosition);

        auto getRolloffFactor() const noexcept { return rolloffFactor; }
        void setRolloffFactor(float newRolloffFactor);

        auto getMinDistance() const noexcept { return minDistance; }
        void setMinDistance(float newMinDistance);

        auto getMaxDistance() const noexcept { return maxDistance; }
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

        PitchScale(const PitchScale&) = delete;
        PitchScale& operator=(const PitchScale&) = delete;
        PitchScale(PitchScale&&) = delete;
        PitchScale& operator=(PitchScale&&) = delete;

        auto getScale() const noexcept { return scale; }
        void setScale(float newScale);

        auto& getScaleRandom() const noexcept { return scaleRandom; }
        void setScaleRandom(const std::pair<float, float>& newScaleRandom);

    private:
        float scale = 1.0F;
        std::pair<float, float> scaleRandom{0.0F, 0.0F};
    };

    class PitchShift final: public Effect
    {
    public:
        PitchShift(Audio& initAudio, float initShift = 1.0F);

        PitchShift(const PitchShift&) = delete;
        PitchShift& operator=(const PitchShift&) = delete;
        PitchShift(PitchShift&&) = delete;
        PitchShift& operator=(PitchShift&&) = delete;

        auto getShift() const noexcept { return shift; }
        void setShift(float newShift);

        auto& getShiftRandom() const noexcept { return shiftRandom; }
        void setShiftRandom(const std::pair<float, float>& newShiftRandom);

    private:
        float shift = 1.0f;
        std::pair<float, float> shiftRandom{0.0F, 0.0F};
    };

    class Reverb final: public Effect
    {
    public:
        Reverb(Audio& initAudio, float initDelay = 0.1F, float initDecay = 0.5F);

        Reverb(const Reverb&) = delete;
        Reverb& operator=(const Reverb&) = delete;
        Reverb(Reverb&&) = delete;
        Reverb& operator=(Reverb&&) = delete;

        auto getDelay() const noexcept { return delay; }
        auto getDecay() const noexcept { return decay; }

    private:
        float delay = 0.1F;
        float decay = 0.5F;
    };

    class LowPass final: public Effect
    {
    public:
        explicit LowPass(Audio& initAudio);

        LowPass(const LowPass&) = delete;
        LowPass& operator=(const LowPass&) = delete;
        LowPass(LowPass&&) = delete;
        LowPass& operator=(LowPass&&) = delete;
    };

    class HighPass final: public Effect
    {
    public:
        explicit HighPass(Audio& initAudio);

        HighPass(const HighPass&) = delete;
        HighPass& operator=(const HighPass&) = delete;
        HighPass(HighPass&&) = delete;
        HighPass& operator=(HighPass&&) = delete;
    };
}

#endif // OUZEL_AUDIO_EFFECTS_HPP
