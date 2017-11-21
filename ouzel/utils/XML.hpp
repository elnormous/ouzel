// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace xml
    {
        class Node
        {
        public:
            enum class Type
            {
                NONE,
                COMMENT,
                CDATA,
                DOCTYPE_DEFINITION,
                PROCESSING_INSTRUCTION,
                TAG,
                TEXT
            };

            Node();

            Type getType() const { return type; }

            const std::string& getValue() const { return value; }

            const std::vector<Node>& getChildren() const { return children; }

            bool parse(const std::vector<uint32_t>& utf32,
                       std::vector<uint32_t>::iterator& iterator);
            bool encode(std::vector<uint8_t>& data) const;

        protected:
            bool skipWhitespaces(const std::vector<uint32_t>& utf32,
                                 std::vector<uint32_t>::iterator& iterator);

            bool parseName(const std::vector<uint32_t>& utf32,
                           std::vector<uint32_t>::iterator& iterator,
                           std::string& result);

            bool parseString(const std::vector<uint32_t>& utf32,
                             std::vector<uint32_t>::iterator& iterator,
                             std::string& result);

            Type type = Type::NONE;

            std::string value;
            std::map<std::string, std::string> attributes;
            std::vector<Node> children;
        };

        class Data
        {
        public:
            Data();
            Data(const std::string& filename, bool preserveComments = false, bool preserveProcessingInstructions = false);
            Data(const std::vector<uint8_t>& data, bool preserveComments = false, bool preserveProcessingInstructions = false);

            bool init(const std::string& filename, bool preserveComments = false, bool preserveProcessingInstructions = false);
            bool init(const std::vector<uint8_t>& data, bool preserveComments = false, bool preserveProcessingInstructions = false);
            
            bool save(const std::string& filename) const;
            bool encode(std::vector<uint8_t>& data) const;

            bool hasBOM() const { return bom; }
            void setBOM(bool newBOM) { bom = newBOM; }

            const std::vector<Node>& getChildren() const { return children; }

        protected:
            bool bom = false;
            std::vector<Node> children;
        };
    } // namespace xml
}
