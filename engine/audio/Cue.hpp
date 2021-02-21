// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_BANK_HPP
#define OUZEL_AUDIO_BANK_HPP

#include <queue>
#include <string>
#include <vector>
#include "Oscillator.hpp"

namespace ouzel::audio
{
    class Audio;
    class Sound;

    struct AttributeDefinition final
    {
        std::string name;
        std::string attribute;
    };

    struct EffectDefinition final
    {
        enum class Type
        {
            delay,
            gain,
            pitchScale,
            pitchShift,
            reverb,
            lowPass,
            highPass
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

    struct SourceDefinition final
    {
        enum class Type
        {
            empty,
            parallel,
            random,
            sequence,
            oscillator,
            silence,
            wavePlayer
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
}

#endif // OUZEL_AUDIO_BANK_HPP
