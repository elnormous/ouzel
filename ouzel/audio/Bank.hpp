// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_BANK_HPP
#define OUZEL_AUDIO_BANK_HPP

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

        struct CueDefinition final
        {
            std::vector<SourceDefinition> sourceDefinitions;
        };

        struct SubmixDefinition final
        {
            std::vector<SubmixDefinition> inputDefinitions;
            std::vector<FilterDefinition> filterDefinitions;
            std::vector<AttributeDefinition> attributeDefinitions;
            std::vector<CueDefinition> cueDefinitions;
        };

        struct BankDefinition final
        {
            std::vector<SubmixDefinition> submixDefinitions;
        };

        class Cue
        {
        public:
            uintptr_t busId;
        };

        class Bank
        {
        public:
            Bank(Audio& initAudio):
                audio(initAudio)
            {
            }

            Bank(Audio& initAudio,
                 const BankDefinition& bankDefinition):
                audio(initAudio),
                definition(bankDefinition)
            {
                // TODO: create command buffer

                for (const SubmixDefinition& submixDefinition : bankDefinition.submixDefinitions)
                    parseSubmixDefinition(submixDefinition);
            }

            Voice createVoice()
            {
                // TODO: create cue
                return Voice(audio);
            }

        private:
            void parseSubmixDefinition(const SubmixDefinition& submixDefinition)
            {
                // TODO: add create bus commands to buffer

                for (const SubmixDefinition& inputDefinition : submixDefinition.inputDefinitions)
                    parseSubmixDefinition(inputDefinition);

                for (const FilterDefinition& filterDefinition : submixDefinition.filterDefinitions)
                {

                }

                for (const AttributeDefinition& attributeDefinition : submixDefinition.attributeDefinitions)
                {

                }

                for (const CueDefinition& cueDefinition : submixDefinition.cueDefinitions)
                {

                }
            }

            Audio& audio;
            BankDefinition definition;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_BANK_HPP
