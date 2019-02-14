// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_XML_HPP
#define OUZEL_UTILS_XML_HPP

#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace xml
    {
        class Data;

        class Node final
        {
            friend Data;
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
            Node(const std::string& val): type(Type::TEXT), value(val) {}

            inline Node& operator=(Type newType)
            {
                type = newType;
                return *this;
            }

            inline Node& operator=(const std::string& val)
            {
                type = Type::TEXT;
                value = val;
                return *this;
            }

            inline Type getType() const { return type; }

            inline const std::string& getValue() const { return value; }
            inline void getValue(const std::string& newValue) { value = newValue; }

            inline const std::vector<Node>& getChildren() const { return children; }

        protected:
            void parse(const std::vector<uint32_t>& str,
                       std::vector<uint32_t>::const_iterator& iterator,
                       bool preserveWhitespaces = false,
                       bool preserveComments = false,
                       bool preserveProcessingInstructions = false);
            void encode(std::vector<uint8_t>& data) const;

        private:
            Type type = Type::NONE;

            std::string value;
            std::map<std::string, std::string> attributes;
            std::vector<Node> children;
        };

        class Data final
        {
        public:
            Data();
            Data(const std::vector<uint8_t>& data,
                 bool preserveWhitespaces = false,
                 bool preserveComments = false,
                 bool preserveProcessingInstructions = false);

            std::vector<uint8_t> encode() const;

            inline bool hasBOM() const { return bom; }
            inline void setBOM(bool newBOM) { bom = newBOM; }

            inline const std::vector<Node>& getChildren() const { return children; }

        private:
            bool bom = false;
            std::vector<Node> children;
        };
    } // namespace xml
} // namespace ouzel

#endif // OUZEL_UTILS_XML_HPP
