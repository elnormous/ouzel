// Ouzel by Elviss Strazdins

#include <stack>
#include "CueLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../audio/Cue.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    CueLoader::CueLoader(Cache& initCache):
        Loader{initCache, Type::cue}
    {
    }

    namespace
    {
        audio::SourceDefinition parseSourceDefinition(const json::Value& value, Cache& cache)
        {
            audio::SourceDefinition sourceDefinition;

            const auto& valueType = value["type"].as<std::string>();

            if (valueType == "Parallel")
                sourceDefinition.type = audio::SourceDefinition::Type::parallel;
            else if (valueType == "Random")
                sourceDefinition.type = audio::SourceDefinition::Type::random;
            else if (valueType == "Sequence")
                sourceDefinition.type = audio::SourceDefinition::Type::sequence;
            else if (valueType == "Oscillator")
            {
                sourceDefinition.type = audio::SourceDefinition::Type::oscillator;

                auto& oscillatorType = value["oscillatorType"].as<std::string>();

                if (oscillatorType == "Sine")
                    sourceDefinition.oscillatorType = audio::Oscillator::Type::sine;
                else if (oscillatorType == "Square")
                    sourceDefinition.oscillatorType = audio::Oscillator::Type::square;
                else if (oscillatorType == "Sawtooth")
                    sourceDefinition.oscillatorType = audio::Oscillator::Type::sawtooth;
                else if (oscillatorType == "Triangle")
                    sourceDefinition.oscillatorType = audio::Oscillator::Type::triangle;

                if (value.hasMember("frequency"))
                    sourceDefinition.length = value["frequency"].as<float>();

                if (value.hasMember("amplitude"))
                    sourceDefinition.length = value["amplitude"].as<float>();

                if (value.hasMember("length"))
                    sourceDefinition.length = value["length"].as<float>();
            }
            else if (valueType == "Silence")
            {
                sourceDefinition.type = audio::SourceDefinition::Type::silence;

                if (value.hasMember("length"))
                    sourceDefinition.length = value["length"].as<float>();
            }
            else if (valueType == "WavePlayer")
            {
                sourceDefinition.type = audio::SourceDefinition::Type::wavePlayer;

                if (value.hasMember("source"))
                    sourceDefinition.sound = cache.getSound(value["source"].as<std::string>());
            }
            else
                throw std::runtime_error("Invalid source type " + valueType);

            if (value.hasMember("effects"))
                for (const json::Value& effectValue : value["effects"])
                {
                    audio::EffectDefinition effectDefinition;

                    const auto& effectType = effectValue["type"].as<std::string>();

                    if (effectType == "Delay")
                        effectDefinition.type = audio::EffectDefinition::Type::delay;
                    else if (effectType == "Gain")
                        effectDefinition.type = audio::EffectDefinition::Type::gain;
                    else if (effectType == "PitchScale")
                        effectDefinition.type = audio::EffectDefinition::Type::pitchScale;
                    else if (effectType == "PitchShift")
                        effectDefinition.type = audio::EffectDefinition::Type::pitchShift;
                    else if (effectType == "Reverb")
                        effectDefinition.type = audio::EffectDefinition::Type::reverb;
                    else if (effectType == "LowPass")
                        effectDefinition.type = audio::EffectDefinition::Type::lowPass;
                    else if (effectType == "HighPass")
                        effectDefinition.type = audio::EffectDefinition::Type::highPass;
                    else
                        throw std::runtime_error("Invalid effect type " + effectType);

                    if (effectValue.hasMember("delay")) effectDefinition.delay = effectValue["delay"].as<float>();
                    if (effectValue.hasMember("gain")) effectDefinition.gain = effectValue["gain"].as<float>();
                    if (effectValue.hasMember("scale")) effectDefinition.scale = effectValue["scale"].as<float>();
                    if (effectValue.hasMember("shift")) effectDefinition.shift = effectValue["shift"].as<float>();
                    if (effectValue.hasMember("decay")) effectDefinition.decay = effectValue["decay"].as<float>();

                    sourceDefinition.effectDefinitions.push_back(effectDefinition);
                }

            if (value.hasMember("sources"))
                for (const json::Value& sourceValue : value["sources"])
                    sourceDefinition.sourceDefinitions.push_back(parseSourceDefinition(sourceValue, cache));

            return sourceDefinition;
        }
    }

    bool CueLoader::loadAsset(Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
                              bool)
    {
        audio::SourceDefinition sourceDefinition;
        const auto d = json::parse(data);

        if (d.hasMember("source"))
            sourceDefinition = parseSourceDefinition(d["source"], cache);

        auto cue = std::make_unique<audio::Cue>(sourceDefinition);

        bundle.setCue(name, std::move(cue));

        return true;
    }
}
