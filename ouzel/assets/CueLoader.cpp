// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stack>
#include "CueLoader.hpp"
#include "Bundle.hpp"
#include "audio/Cue.hpp"
#include "utils/Json.hpp"

namespace ouzel
{
    namespace assets
    {
        CueLoader::CueLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        static audio::SourceDefinition parseSourceDefinition(const json::Value& value)
        {
            audio::SourceDefinition sourceDefinition;

            if (value["type"].as<std::string>() == "Parallel")
                sourceDefinition.type = audio::SourceDefinition::Type::Parallel;
            else if (value["type"].as<std::string>() == "Random")
                sourceDefinition.type = audio::SourceDefinition::Type::Random;
            else if (value["type"].as<std::string>() == "Sequence")
                sourceDefinition.type = audio::SourceDefinition::Type::Sequence;
            else if (value["type"].as<std::string>() == "Oscillator")
                sourceDefinition.type = audio::SourceDefinition::Type::Oscillator;
            else if (value["type"].as<std::string>() == "Silence")
                sourceDefinition.type = audio::SourceDefinition::Type::Silence;
            else if (value["type"].as<std::string>() == "WavePlayer")
                sourceDefinition.type = audio::SourceDefinition::Type::WavePlayer;

            if (value.hasMember("effects"))
                for (const json::Value& effectValue : value["effects"])
                {
                    audio::EffectDefinition effectDefinition;

                    if (effectValue["type"].as<std::string>() == "Delay")
                        effectDefinition.type = audio::EffectDefinition::Type::Delay;
                    else if (effectValue["type"].as<std::string>() == "Gain")
                        effectDefinition.type = audio::EffectDefinition::Type::Gain;
                    else if (effectValue["type"].as<std::string>() == "PitchScale")
                        effectDefinition.type = audio::EffectDefinition::Type::PitchScale;
                    else if (effectValue["type"].as<std::string>() == "PitchShift")
                        effectDefinition.type = audio::EffectDefinition::Type::PitchShift;
                    else if (effectValue["type"].as<std::string>() == "Reverb")
                        effectDefinition.type = audio::EffectDefinition::Type::Reverb;
                    else if (effectValue["type"].as<std::string>() == "LowPass")
                        effectDefinition.type = audio::EffectDefinition::Type::LowPass;
                    else if (effectValue["type"].as<std::string>() == "HighPass")
                        effectDefinition.type = audio::EffectDefinition::Type::HighPass;

                    sourceDefinition.effectDefinitions.push_back(effectDefinition);
                }

            if (value.hasMember("sources"))
                for (const json::Value& sourceValue : value["sources"])
                    sourceDefinition.sourceDefinitions.push_back(parseSourceDefinition(sourceValue));

            return sourceDefinition;
        };

        bool CueLoader::loadAsset(Bundle& bundle,
                                  const std::string& name,
                                  const std::vector<uint8_t>& data,
                                  bool)
        {
            std::vector<audio::SourceDefinition> sourceDefinitions;
            const json::Data d(data);

            if (d.hasMember("sources"))
                for (const json::Value& sourceValue : d["sources"])
                    sourceDefinitions.push_back(parseSourceDefinition(sourceValue));

            auto cue = std::make_unique<audio::Cue>(sourceDefinitions);

            bundle.setCue(name, std::move(cue));

            return true;
        }
    } // namespace assets
} // namespace ouzel
