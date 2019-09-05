// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EFFECTS_HPP
#define OUZEL_AUDIO_EFFECTS_HPP

#include <cfloat>
#include <utility>
#include "audio/Effect.hpp"
#include "math/Vector.hpp"
#include "scene/Component.hpp"

namespace smb
{
    template <uint32_t fftFrameSize, uint32_t oversamp> class PitchShift;
}

namespace ouzel
{
    namespace audio
    {
        class Delay final: public Effect
        {
        public:
            Delay(Audio& initAudio, float initDelay = 0.0F);

            Delay(const Delay&) = delete;
            Delay& operator=(const Delay&) = delete;
            Delay(Delay&&) = delete;
            Delay& operator=(Delay&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;

            inline auto getDelay() const noexcept { return delay; }
            void setDelay(float newDelay);

            inline const std::pair<float, float>& getDelayRandom() const noexcept { return delayRandom; }
            void setDelayRandom(const std::pair<float, float>& newDelayRandom);

        private:
            float delay = 0.0F;
            std::pair<float, float> delayRandom{0.0F, 0.0F};
            std::vector<float> buffer;
        };

        class Gain final: public Effect
        {
        public:
            Gain(Audio& initAudio, float initGain = 0.0F);

            Gain(const Gain&) = delete;
            Gain& operator=(const Gain&) = delete;
            Gain(Gain&&) = delete;
            Gain& operator=(Gain&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;

            inline auto getGain() const noexcept { return gain; }
            void setGain(float newGain);

            inline const std::pair<float, float>& getGainRandom() const noexcept { return gainRandom; }
            void setGainRandom(const std::pair<float, float>& newGainRandom);

        private:
            float gain = 0.0F; // dB
            float gainFactor = 1.0F;
            std::pair<float, float> gainRandom{0.0F, 0.0F};
        };

        class Panner final: public Effect
        {
        public:
            explicit Panner(Audio& initAudio);

            Panner(const Panner&) = delete;
            Panner& operator=(const Panner&) = delete;
            Panner(Panner&&) = delete;
            Panner& operator=(Panner&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;

            inline auto& getPosition() const noexcept { return position; }
            void setPosition(const Vector3F& newPosition);

        private:
            Vector3F position;
        };

        class PitchScale final: public Effect
        {
        public:
            PitchScale(Audio& initAudio, float initScale = 1.0F);

            PitchScale(const PitchScale&) = delete;
            PitchScale& operator=(const PitchScale&) = delete;
            PitchScale(PitchScale&&) = delete;
            PitchScale& operator=(PitchScale&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;

            inline auto getScale() const noexcept { return scale; }
            void setScale(float newScale);

            inline const std::pair<float, float>& getScaleRandom() const noexcept { return scaleRandom; }
            void setScaleRandom(const std::pair<float, float>& newScaleRandom);

        private:
            float scale = 1.0F;
            std::pair<float, float> scaleRandom{0.0F, 0.0F};
            std::vector<smb::PitchShift<1024, 4>> pitchShift;
        };

        class PitchShift final: public Effect
        {
        public:
            PitchShift(Audio& initAudio, float initShift = 1.0F);

            PitchShift(const PitchShift&) = delete;
            PitchShift& operator=(const PitchShift&) = delete;
            PitchShift(PitchShift&&) = delete;
            PitchShift& operator=(PitchShift&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;

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

            Reverb(const Reverb&) = delete;
            Reverb& operator=(const Reverb&) = delete;
            Reverb(Reverb&&) = delete;
            Reverb& operator=(Reverb&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;

            inline auto getDelay() const noexcept { return delay; }
            inline auto getDecay() const noexcept { return decay; }

        private:
            float delay = 0.1F;
            float decay = 0.5F;
            std::vector<std::vector<float>> buffers;
        };

        class LowPass final: public Effect
        {
        public:
            explicit LowPass(Audio& initAudio);

            LowPass(const LowPass&) = delete;
            LowPass& operator=(const LowPass&) = delete;
            LowPass(LowPass&&) = delete;
            LowPass& operator=(LowPass&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;
        };

        class HighPass final: public Effect
        {
        public:
            explicit HighPass(Audio& initAudio);

            HighPass(const HighPass&) = delete;
            HighPass& operator=(const HighPass&) = delete;
            HighPass(HighPass&&) = delete;
            HighPass& operator=(HighPass&&) = delete;

            void process(uint32_t frames, uint16_t channels, uint32_t sampleRate,
                         std::vector<float>& samples) override;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EFFECTS_HPP
