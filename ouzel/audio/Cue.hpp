// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_BANK_HPP
#define OUZEL_AUDIO_BANK_HPP

#include <queue>
#include <string>
#include <vector>
#include "Voice.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        struct AttributeDefinition final
        {
            std::string name;
            std::string attribute;
        };

        struct FilterDefinition final
        {
            std::vector<AttributeDefinition> attributeDefinitions;
        };

        struct SourceDefinition final
        {
            std::vector<SourceDefinition> sourceDefinitions;
            std::vector<FilterDefinition> filterDefinitions;
            std::vector<AttributeDefinition> attributeDefinitions;
        };

        class Cue
        {
        public:
            Cue() {}
            explicit Cue(const std::vector<SourceDefinition>& initSourceDefinitions):
                sourceDefinitions(initSourceDefinitions)
            {
            }

            const std::vector<SourceDefinition>& getSourceDefinitions() const
            {
                return sourceDefinitions;
            }

        private:
            std::vector<SourceDefinition> sourceDefinitions;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_BANK_HPP
