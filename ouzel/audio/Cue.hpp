// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_BANK_HPP
#define OUZEL_AUDIO_BANK_HPP

#include <queue>
#include <string>
#include <vector>
#include "audio/Effect.hpp"
#include "audio/Oscillator.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Sound;

        struct SourceDefinition final
        {
            enum class Type
            {
                Empty,
                Parallel,
                Random,
                Sequence,
                Oscillator,
                Silence,
                WavePlayer
            };

            Type type;
            std::vector<SourceDefinition> sourceDefinitions;
            std::vector<EffectDefinition> effectDefinitions;
            std::vector<AttributeDefinition> attributeDefinitions;
            Oscillator::Type oscillatorType;
            float frequency = 0.0F;
            float amplitude = 0.0F;
            float length = 0.0F;
            const Sound* sound = nullptr;
        };

        class Cue
        {
        public:
            Cue() {}
            explicit Cue(const SourceDefinition& initSourceDefinition):
                sourceDefinition(initSourceDefinition)
            {
            }

            const SourceDefinition& getSourceDefinition() const
            {
                return sourceDefinition;
            }

        private:
            SourceDefinition sourceDefinition;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_BANK_HPP
