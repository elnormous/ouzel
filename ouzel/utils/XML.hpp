// Copyright (C) 2018 Elviss Strazdins
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
                TYPE_DECLARATION,
                PROCESSING_INSTRUCTION,
                TAG,
                TEXT
            };

            Node() {}
            Node(Type initType): type(initType) {}

            Node& operator=(Type newType)
            {
                type = newType;
                return *this;
            }

            Type getType() const { return type; }

            const std::string& getValue() const { return value; }
            void getValue(const std::string& newValue) { value = newValue; }

            const std::vector<Node>& getChildren() const { return children; }

            bool parse(const std::vector<uint32_t>& str,
                       std::vector<uint32_t>::const_iterator& iterator,
                       bool preserveWhitespaces = false,
                       bool preserveComments = false,
                       bool preserveProcessingInstructions = false);
            bool encode(std::vector<uint8_t>& data) const;

        protected:
            Type type = Type::NONE;

            std::string value;
            std::map<std::string, std::string> attributes;
            std::vector<Node> children;
        };

        class Data
        {
        public:
            Data();
            Data(const std::string& filename,
                 bool preserveWhitespaces = false,
                 bool preserveComments = false,
                 bool preserveProcessingInstructions = false);
            Data(const std::vector<uint8_t>& data,
                 bool preserveWhitespaces = false,
                 bool preserveComments = false,
                 bool preserveProcessingInstructions = false);

            bool init(const std::string& filename,
                      bool preserveWhitespaces = false,
                      bool preserveComments = false,
                      bool preserveProcessingInstructions = false);
            bool init(const std::vector<uint8_t>& data,
                      bool preserveWhitespaces = false,
                      bool preserveComments = false,
                      bool preserveProcessingInstructions = false);

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
} // namespace ouzel
