// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <stack>
#include "CueLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../audio/Cue.hpp"
#include "../utils/Json.hpp"

namespace ouzel
{
    namespace assets
    {
        CueLoader::CueLoader(Cache& initCache):
            Loader(initCache, Loader::Cue)
        {
        }

        namespace
        {
            audio::SourceDefinition parseSourceDefinition(const json::Value& value, Cache& cache)
            {
                audio::SourceDefinition sourceDefinition;

                auto& valueType = value["type"].as<std::string>();

                if (valueType == "Parallel")
                    sourceDefinition.type = audio::SourceDefinition::Type::Parallel;
                else if (valueType == "Random")
                    sourceDefinition.type = audio::SourceDefinition::Type::Random;
                else if (valueType == "Sequence")
                    sourceDefinition.type = audio::SourceDefinition::Type::Sequence;
                else if (valueType == "Oscillator")
                {
                    sourceDefinition.type = audio::SourceDefinition::Type::Oscillator;

                    auto& oscillatorType = value["oscillatorType"].as<std::string>();

                    if (oscillatorType == "Sine")
                        sourceDefinition.oscillatorType = audio::Oscillator::Type::Sine;
                    else if (oscillatorType == "Square")
                        sourceDefinition.oscillatorType = audio::Oscillator::Type::Square;
                    else if (oscillatorType == "Sawtooth")
                        sourceDefinition.oscillatorType = audio::Oscillator::Type::Sawtooth;
                    else if (oscillatorType == "Triangle")
                        sourceDefinition.oscillatorType = audio::Oscillator::Type::Triangle;

                    if (value.hasMember("frequency"))
                        sourceDefinition.length = value["frequency"].as<float>();

                    if (value.hasMember("amplitude"))
                        sourceDefinition.length = value["amplitude"].as<float>();

                    if (value.hasMember("length"))
                        sourceDefinition.length = value["length"].as<float>();
                }
                else if (valueType == "Silence")
                {
                    sourceDefinition.type = audio::SourceDefinition::Type::Silence;

                    if (value.hasMember("length"))
                        sourceDefinition.length = value["length"].as<float>();
                }
                else if (valueType == "WavePlayer")
                {
                    sourceDefinition.type = audio::SourceDefinition::Type::WavePlayer;

                    if (value.hasMember("source"))
                        sourceDefinition.sound = cache.getSound(value["source"].as<std::string>());
                }
                else
                    throw std::runtime_error("Invalid source type " + valueType);

                if (value.hasMember("effects"))
                    for (const json::Value& effectValue : value["effects"])
                    {
                        audio::EffectDefinition effectDefinition;

                        auto& effectType = effectValue["type"].as<std::string>();

                        if (effectType == "Delay")
                            effectDefinition.type = audio::EffectDefinition::Type::Delay;
                        else if (effectType == "Gain")
                            effectDefinition.type = audio::EffectDefinition::Type::Gain;
                        else if (effectType == "PitchScale")
                            effectDefinition.type = audio::EffectDefinition::Type::PitchScale;
                        else if (effectType == "PitchShift")
                            effectDefinition.type = audio::EffectDefinition::Type::PitchShift;
                        else if (effectType == "Reverb")
                            effectDefinition.type = audio::EffectDefinition::Type::Reverb;
                        else if (effectType == "LowPass")
                            effectDefinition.type = audio::EffectDefinition::Type::LowPass;
                        else if (effectType == "HighPass")
                            effectDefinition.type = audio::EffectDefinition::Type::HighPass;
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
                                  const std::vector<std::uint8_t>& data,
                                  bool)
        {
            audio::SourceDefinition sourceDefinition;
            const json::Data d(data);

            if (d.hasMember("source"))
                sourceDefinition = parseSourceDefinition(d["source"], cache);

            auto cue = std::make_unique<audio::Cue>(sourceDefinition);

            bundle.setCue(name, std::move(cue));

            return true;
        }
    } // namespace assets
} // namespace ouzel
