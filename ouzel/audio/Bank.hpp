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

                std::queue<const SubmixDefinition*> submixDefinitions;

                for (const SubmixDefinition& submixDefinition : bankDefinition.submixDefinitions)
                    submixDefinitions.push(&submixDefinition);

                while (!submixDefinitions.empty())
                {
                    const SubmixDefinition* submixDefinition = submixDefinitions.front();
                    submixDefinitions.pop();

                    // TODO: create unique_ptr with the Submix

                    for (const FilterDefinition& filterDefinition : submixDefinition->filterDefinitions)
                    {
                        // TODO: create and add filters to the submix
                        // TODO: store attribue in the attribute map of the Bank
                    }

                    for (const AttributeDefinition& attributeDefinition : submixDefinition->attributeDefinitions)
                    {
                        // TODO: store attribue in the attribute map of the Bank
                    }

                    // TODO: add initObject command to command buffer
                    // TODO: add addInput command to command buffer

                    for (const CueDefinition& cueDefinition : submixDefinition->cueDefinitions)
                    {
                        // TODO: create cue and store it in the cue map of the Bank
                    }

                    for (const SubmixDefinition& inputDefinition : submixDefinition->inputDefinitions)
                        submixDefinitions.push(&inputDefinition);
                }
            }

            // TODO: add getter for cues

            Voice createVoice()
            {
                // TODO: create cue
                return Voice(audio);
            }

        private:
            Audio& audio;
            BankDefinition definition;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_BANK_HPP
