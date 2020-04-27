// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_XML_HPP
#define OUZEL_UTILS_XML_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include "Utf8.hpp"

namespace ouzel
{
    namespace xml
    {
        class ParseError final: public std::logic_error
        {
        public:
            explicit ParseError(const std::string& str): std::logic_error(str) {}
            explicit ParseError(const char* str): std::logic_error(str) {}
        };

        inline namespace detail
        {
            constexpr std::uint8_t utf8ByteOrderMark[] = {0xEF, 0xBB, 0xBF};

            constexpr auto isWhitespace(const char32_t c) noexcept
            {
                return c == ' ' || c == '\t' || c == '\r' || c == '\n';
            }

            constexpr auto isNameStartChar(const char32_t c) noexcept
            {
                return (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    c == ':' || c == '_' ||
                    (c >= 0xC0 && c <= 0xD6) ||
                    (c >= 0xD8 && c <= 0xF6) ||
                    (c >= 0xF8 && c <= 0x2FF) ||
                    (c >= 0x370 && c <= 0x37D) ||
                    (c >= 0x37F && c <= 0x1FFF) ||
                    (c >= 0x200C && c <= 0x200D) ||
                    (c >= 0x2070 && c <= 0x218F);
            }

            constexpr auto isNameChar(const char32_t c) noexcept
            {
                return isNameStartChar(c) ||
                    c == '-' || c == '.' ||
                    (c >= '0' && c <= '9') ||
                    c == 0xB7 ||
                    (c >= 0x0300 && c <= 0x036F) ||
                    (c >= 0x203F && c <= 0x2040);
            }

            inline void skipWhitespaces(std::u32string::const_iterator& iterator,
                                        std::u32string::const_iterator end)
            {
                while (iterator != end)
                    if (isWhitespace(*iterator))
                        ++iterator;
                    else
                        break;
            }

            inline std::string parseName(std::u32string::const_iterator& iterator,
                                         std::u32string::const_iterator end)
            {
                std::string result;

                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                if (!isNameStartChar(*iterator))
                    throw ParseError("Invalid name start");

                for (;;)
                {
                    if (iterator == end)
                        throw ParseError("Unexpected end of data");

                    if (!isNameChar(*iterator))
                        break;
                    else
                    {
                        result += utf8::fromUtf32(*iterator);
                        ++iterator;
                    }
                }

                return result;
            }

            inline std::string parseEntity(std::u32string::const_iterator& iterator,
                                           std::u32string::const_iterator end)
            {
                std::string result;

                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                if (*iterator != '&')
                    throw ParseError("Expected an ampersand");

                std::string value;

                for (;;)
                {
                    if (++iterator == end)
                        throw ParseError("Unexpected end of data");

                    if (*iterator == ';')
                    {
                        ++iterator;
                        break;
                    }
                    else
                        value.push_back(static_cast<char>(*iterator));
                }

                if (value.empty())
                    throw ParseError("Invalid entity");

                if (value == "quot")
                    result = "\"";
                else if (value == "amp")
                    result = "&";
                else if (value == "apos")
                    result = "'";
                else if (value == "lt")
                    result = "<";
                else if (value == "gt")
                    result = ">";
                else if (value[0] == '#')
                {
                    if (value.length() < 2)
                        throw ParseError("Invalid entity");

                    char32_t c = 0;

                    if (value[1] == 'x') // hex value
                    {
                        if (value.length() != 2 + 4)
                            throw ParseError("Invalid entity");

                        for (std::size_t i = 0; i < 4; ++i)
                        {
                            std::uint8_t code = 0;

                            if (value[i + 2] >= '0' && value[i + 2] <= '9')
                                code = static_cast<std::uint8_t>(value[i + 2]) - '0';
                            else if (value[i + 2] >= 'a' && value[i + 2] <='f')
                                code = static_cast<std::uint8_t>(value[i + 2]) - 'a' + 10;
                            else if (value[i + 2] >= 'A' && value[i + 2] <='F')
                                code = static_cast<std::uint8_t>(value[i + 2]) - 'A' + 10;
                            else
                                throw ParseError("Invalid character code");

                            c = (c << 4) | code;
                        }
                    }
                    else
                    {
                        if (value.length() != 1 + 4)
                            throw ParseError("Invalid entity");

                        for (std::size_t i = 0; i < 4; ++i)
                        {
                            std::uint8_t code = 0;

                            if (value[i + 1] >= '0' && value[i + 1] <= '9')
                                code = static_cast<std::uint8_t>(value[i + 1]) - '0';
                            else
                                throw ParseError("Invalid character code");

                            c = c * 10 + code;
                        }
                    }

                    result = utf8::fromUtf32(c);
                }
                else
                    throw ParseError("Invalid entity");

                return result;
            }

            inline std::string parseString(std::u32string::const_iterator& iterator,
                                           std::u32string::const_iterator end)
            {
                std::string result;

                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                if (*iterator != '"' && *iterator != '\'')
                    throw ParseError("Expected quotes");

                auto quotes = *iterator;

                ++iterator;

                for (;;)
                {
                    if (iterator == end)
                        throw ParseError("Unexpected end of data");

                    if (*iterator == quotes)
                    {
                        ++iterator;
                        break;
                    }
                    else if (*iterator == '&')
                    {
                        std::string entity = parseEntity(iterator, end);
                        result += entity;
                    }
                    else
                    {
                        result += utf8::fromUtf32(*iterator);
                        ++iterator;
                    }
                }

                return result;
            }

            inline void encodeString(std::string& data,
                                     const std::u32string& str)
            {
                for (const char32_t c : str)
                {
                    switch (c)
                    {
                        case '"':
                            data.insert(data.end(), {'&', 'q', 'u', 'o', 't', ';'});
                            break;
                        case '&':
                            data.insert(data.end(), {'&', 'a', 'm', 'p', ';'});
                            break;
                        case '\'':
                            data.insert(data.end(), {'&', 'a', 'p', 'o', 's', ';'});
                            break;
                        case '<':
                            data.insert(data.end(), {'&', 'l', 't', ';'});
                            break;
                        case '>':
                            data.insert(data.end(), {'&', 'g', 't', ';'});
                            break;
                        default:
                            const std::string encoded = utf8::fromUtf32(c);
                            data.insert(data.end(), encoded.begin(), encoded.end());
                            break;
                    }
                }
            }
        }

        class Data;

        class Node final
        {
            friend Data;
        public:
            enum class Type
            {
                Comment,
                CData,
                TypeDeclaration,
                ProcessingInstruction,
                Tag,
                Text
            };

            Node() = default;
            Node(Type initType): type(initType) {}
            Node(const std::string& val): type(Type::Text), value(val) {}

            Node& operator=(Type newType) noexcept
            {
                type = newType;
                return *this;
            }

            Node& operator=(const std::string& val)
            {
                type = Type::Text;
                value = val;
                return *this;
            }

            auto getType() const noexcept { return type; }

            auto& getValue() const noexcept { return value; }
            void setValue(const std::string& newValue) { value = newValue; }

            auto& getChildren() const noexcept { return children; }

            std::vector<Node>::iterator begin() { return children.begin(); }
            std::vector<Node>::iterator end() { return children.end(); }

            std::vector<Node>::const_iterator begin() const { return children.begin(); }
            std::vector<Node>::const_iterator end() const { return children.end(); }

        protected:
            void parse(std::u32string::const_iterator& iterator,
                       std::u32string::const_iterator end,
                       bool preserveWhitespaces = false,
                       bool preserveComments = false,
                       bool preserveProcessingInstructions = false)
            {
                if (iterator == end)
                    throw ParseError("Unexpected end of data");

                if (*iterator == '<')
                {
                    if (++iterator == end)
                        throw ParseError("Unexpected end of data");

                    if (*iterator == '!') // <!
                    {
                        if (++iterator == end)
                            throw ParseError("Unexpected end of data");

                        if (*iterator == '-') // <!-
                        {
                            if (++iterator == end)
                                throw ParseError("Unexpected end of data");

                            if (*iterator != '-') // <!--
                                throw ParseError("Expected a comment");

                            for (;;)
                            {
                                if (std::distance(++iterator, end) < 3)
                                    throw ParseError("Unexpected end of data");

                                if (*iterator == '-')
                                {
                                    if (*(iterator + 1) == '-') // --
                                    {
                                        iterator += 2;

                                        if (*iterator == '>') // -->
                                        {
                                            ++iterator;
                                            break;
                                        }
                                        else
                                            throw ParseError("Unexpected double-hyphen inside comment");
                                    }
                                }

                                value += utf8::fromUtf32(*iterator);
                            }

                            type = Type::Comment;
                        }
                        else if (*iterator == '[') // <![
                        {
                            ++iterator;
                            std::string name;
                            name = parseName(iterator, end);

                            if (name != "CDATA")
                                throw ParseError("Expected CDATA");

                            if (iterator == end)
                                throw ParseError("Unexpected end of data");

                            if (*iterator != '[')
                                throw ParseError("Expected a left bracket");

                            for (;;)
                            {
                                if (std::distance(++iterator, end) < 3)
                                    throw ParseError("Unexpected end of data");

                                if (*iterator == ']' &&
                                    *(iterator + 1) == ']' &&
                                    *(iterator + 2) == '>')
                                {
                                    iterator += 3;
                                    break;
                                }

                                value += utf8::fromUtf32(*iterator);
                            }

                            type = Type::CData;
                        }
                        else
                            throw ParseError("Type declarations are not supported");
                    }
                    else if (*iterator == '?') // <?
                    {
                        ++iterator;
                        value = parseName(iterator, end);

                        for (;;)
                        {
                            skipWhitespaces(iterator, end);

                            if (iterator == end)
                                throw ParseError("Unexpected end of data");

                            if (*iterator == '?')
                            {
                                if (++iterator == end)
                                    throw ParseError("Unexpected end of data");

                                if (*iterator != '>') // ?>
                                    throw ParseError("Expected a right angle bracket");

                                ++iterator;
                                break;
                            }

                            std::string attribute = parseName(iterator, end);

                            skipWhitespaces(iterator, end);

                            if (iterator == end)
                                throw ParseError("Unexpected end of data");

                            if (*iterator != '=')
                                throw ParseError("Expected an equal sign");

                            ++iterator;

                            skipWhitespaces(iterator, end);
                            attributes[attribute] = parseString(iterator, end);
                        }

                        type = Type::ProcessingInstruction;
                    }
                    else // <
                    {
                        value = parseName(iterator, end);

                        bool tagClosed = false;

                        for (;;)
                        {
                            skipWhitespaces(iterator, end);

                            if (iterator == end)
                                throw ParseError("Unexpected end of data");

                            if (*iterator == '>')
                            {
                                ++iterator;
                                break;
                            }
                            else if (*iterator == '/')
                            {
                                if (++iterator == end)
                                    throw ParseError("Unexpected end of data");

                                if (*iterator != '>') // />
                                    throw ParseError("Expected a right angle bracket");

                                tagClosed = true;
                                ++iterator;
                                break;
                            }

                            std::string attribute;
                            attribute = parseName(iterator, end);

                            skipWhitespaces(iterator, end);

                            if (iterator == end)
                                throw ParseError("Unexpected end of data");

                            if (*iterator != '=')
                                throw ParseError("Expected an equal sign");

                            ++iterator;

                            skipWhitespaces(iterator, end);

                            attributes[attribute] = parseString(iterator, end);
                        }

                        if (!tagClosed)
                        {
                            for (;;)
                            {
                                if (!preserveWhitespaces) skipWhitespaces(iterator, end);

                                if (iterator == end)
                                    throw ParseError("Unexpected end of data");

                                if (*iterator == '<' &&
                                    iterator + 1 != end &&
                                    *(iterator + 1) == '/')
                                {
                                    ++iterator; // skip the left angle bracket
                                    ++iterator; // skip the slash

                                    std::string tag = parseName(iterator, end);

                                    if (tag != value)
                                        throw ParseError("Tag not closed properly");

                                    if (iterator == end)
                                        throw ParseError("Unexpected end of data");

                                    if (*iterator != '>')
                                        throw ParseError("Expected a right angle bracket");

                                    ++iterator;

                                    break;
                                }
                                else
                                {
                                    Node node;
                                    node.parse(iterator, end, preserveWhitespaces, preserveComments, preserveProcessingInstructions);

                                    if ((preserveComments || node.getType() != Type::Comment) &&
                                        (preserveProcessingInstructions || node.getType() != Type::ProcessingInstruction))
                                        children.push_back(node);
                                }
                            }
                        }

                        type = Type::Tag;
                    }
                }
                else
                {
                    for (;;)
                    {
                        if (iterator == end || // end of a file
                            *iterator == '<') // start of a tag
                            break;
                        else if (*iterator == '&')
                        {
                            std::string entity = parseEntity(iterator, end);
                            value += entity;
                        }
                        else
                        {
                            value += utf8::fromUtf32(*iterator);
                            ++iterator;
                        }
                    }

                    type = Type::Text;
                }
            }

            void encode(std::string& data) const
            {
                switch (type)
                {
                    case Node::Type::Comment:
                        data.insert(data.end(), {'<', '!', '-', '-'});
                        data.insert(data.end(), value.begin(), value.end());
                        data.insert(data.end(), {'-', '-', '>'});
                        break;
                    case Node::Type::CData:
                        data.insert(data.end(), {'<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['});
                        data.insert(data.end(), value.begin(), value.end());
                        data.insert(data.end(), {']', ']', '>'});
                        break;
                    case Node::Type::TypeDeclaration:
                        throw ParseError("Type declarations are not supported");
                    case Node::Type::ProcessingInstruction:
                        data.insert(data.end(), {'<', '?'});
                        data.insert(data.end(), value.begin(), value.end());

                        if (attributes.empty())
                            data.insert(data.end(), ' ');
                        else
                        {
                            for (const auto& attribute : attributes)
                            {
                                data.insert(data.end(), attribute.first.begin(), attribute.first.end());
                                data.insert(data.end(), {'=', '"'});
                                encodeString(data, utf8::toUtf32(attribute.second));
                                data.insert(data.end(), '"');
                            }
                        }

                        data.insert(data.end(), {'?', '>'});
                        break;
                    case Node::Type::Tag:
                        data.insert(data.end(), '<');
                        data.insert(data.end(), value.begin(), value.end());

                        if (attributes.empty())
                            data.insert(data.end(), ' ');
                        else
                        {
                            for (const auto& attribute : attributes)
                            {
                                data.insert(data.end(), attribute.first.begin(), attribute.first.end());
                                data.insert(data.end(), {'=', '"'});
                                data.insert(data.end(), attribute.second.begin(), attribute.second.end());
                                data.insert(data.end(), '"');
                            }
                        }

                        if (children.empty())
                            data.insert(data.end(), {'/', '>'});
                        else
                        {
                            data.insert(data.end(), '>');

                            for (const Node& node : children)
                                node.encode(data);

                            data.insert(data.end(), {'<', '/'});
                            data.insert(data.end(), value.begin(), value.end());
                            data.insert(data.end(), '>');
                        }
                        break;
                    case Node::Type::Text:
                        encodeString(data, utf8::toUtf32(value));
                        break;
                    default:
                        throw ParseError("Unknown node type");
                }

                for (const Node& node : children)
                    node.encode(data);
            }

        private:
            Type type;

            std::string value;
            std::map<std::string, std::string> attributes;
            std::vector<Node> children;
        };

        class Data final
        {
        public:
            Data() = default;

            template <class T>
            Data(const T& data,
                 bool preserveWhitespaces = false,
                 bool preserveComments = false,
                 bool preserveProcessingInstructions = false):
                Data(std::begin(data), std::end(data), preserveWhitespaces,
                     preserveComments, preserveProcessingInstructions)
            {}

            template <class Iterator>
            Data(Iterator begin, Iterator end,
                 bool preserveWhitespaces = false,
                 bool preserveComments = false,
                 bool preserveProcessingInstructions = false)
            {
                byteOrderMark = hasByteOrderMark(begin, end);

                const std::u32string str = utf8::toUtf32(byteOrderMark ? begin + 3 : begin, end);
                auto iterator = str.begin();
                bool rootTagFound = false;

                for (;;)
                {
                    if (!preserveWhitespaces) skipWhitespaces(iterator, str.end());

                    if (iterator == str.end()) break;

                    Node node;
                    node.parse(iterator, str.end(),
                               preserveWhitespaces,
                               preserveComments,
                               preserveProcessingInstructions);

                    if ((preserveComments || node.getType() != Node::Type::Comment) &&
                        (preserveProcessingInstructions || node.getType() != Node::Type::ProcessingInstruction))
                    {
                        children.push_back(node);

                        if (node.getType() == Node::Type::Tag)
                        {
                            if (rootTagFound)
                                throw ParseError("Multiple root tags found");
                            else
                                rootTagFound = true;
                        }
                    }
                }

                if (!rootTagFound)
                    throw ParseError("No root tag found");
            }

            std::string encode() const
            {
                std::string result;

                if (byteOrderMark) result.assign(std::begin(utf8ByteOrderMark), std::end(utf8ByteOrderMark));

                for (const Node& node : children)
                    node.encode(result);

                return result;
            }

            bool hasByteOrderMark() const noexcept { return byteOrderMark; }
            void setByteOrderMark(const bool newByteOrderMark) noexcept { byteOrderMark = newByteOrderMark; }

            auto& getChildren() const noexcept { return children; }

        private:
            template <class Iterator>
            static bool hasByteOrderMark(Iterator begin, Iterator end) noexcept
            {
                for (auto i = std::begin(utf8ByteOrderMark); i != std::end(utf8ByteOrderMark); ++i, ++begin)
                    if (begin == end || *begin != *i)
                        return false;
                return true;
            }

            bool byteOrderMark = false;
            std::vector<Node> children;
        };
    } // namespace xml
} // namespace ouzel

#endif // OUZEL_UTILS_XML_HPP
