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

            if (value.hasMember("attributes"))
                for (const json::Value& attributeValue : value["attributes"])
                {
                    audio::AttributeDefinition attributeDefinition;
                    sourceDefinition.attributeDefinitions.push_back(attributeDefinition);
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

            auto cue = std::make_shared<audio::Cue>(sourceDefinitions);

            bundle.setCue(name, cue);

            return true;
        }
    } // namespace assets
} // namespace ouzel
