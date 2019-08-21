// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_EFFECT_HPP
#define OUZEL_AUDIO_EFFECT_HPP

#include <cstdint>
#include "audio/Node.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Mix;

        struct AttributeDefinition final
        {
            std::string name;
            std::string attribute;
        };

        struct EffectDefinition final
        {
            enum class Type
            {
                Delay,
                Gain,
                PitchScale,
                PitchShift,
                Reverb,
                LowPass,
                HighPass
            };

            Type type;
            std::vector<AttributeDefinition> attributeDefinitions;
            float delay = 0.0F;
            float gain = 0.0F;
            float scale = 1.0F;
            float shift = 1.0f;
            float decay = 0.0F;
            std::pair<float, float> delayRandom{0.0F, 0.0F};
            std::pair<float, float> gainRandom{0.0F, 0.0F};
            std::pair<float, float> scaleRandom{0.0F, 0.0F};
            std::pair<float, float> shiftRandom{0.0F, 0.0F};
        };
        
        class Effect: public Node
        {
            friend Mix;
        public:
            Effect(Audio& initAudio,
                   uintptr_t initProcessorId);
            virtual ~Effect();

            Effect(const Effect&) = delete;
            Effect& operator=(const Effect&) = delete;
            Effect(Effect&&) = delete;
            Effect& operator=(Effect&&) = delete;

            inline auto getProcessorId() const noexcept { return processorId; }

            inline auto isEnabled() const noexcept { return enabled; }
            void setEnabled(bool newEnabled);

        protected:
            Audio& audio;
            uintptr_t processorId = 0;
            Mix* mix = nullptr;
            bool enabled = true;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_EFFECT_HPP
