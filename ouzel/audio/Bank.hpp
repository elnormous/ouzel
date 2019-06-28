// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_BANK_HPP
#define OUZEL_AUDIO_BANK_HPP

#include <string>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        struct AttributeDefinition final
        {
            std::string name;
            std::string attribute;
        };

        struct FilterDefinition final
        {
            std::vector<AttributeDefinition> attributeDefinitions;
        };

        struct BusDefinition final
        {
            std::vector<BusDefinition> inputDefinitions;
            std::vector<FilterDefinition> filterDefinitions;
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
            // TODO: bus ID
            std::vector<SourceDefinition> sourceDefinitions;
        };

        struct BankDefinition final
        {
            std::vector<BusDefinition> busDefinitions;
            std::vector<CueDefinition> cueDefinitions;
        };

        class Bank
        {
        public:
            Bank(const BankDefinition& bankDefinition):
                definition(bankDefinition)
            {
            }

        private:
            BankDefinition definition;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_BANK_HPP
