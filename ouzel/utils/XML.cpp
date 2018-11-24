// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <stdexcept>
#include "XML.hpp"
#include "Utils.hpp"

namespace ouzel
{
    namespace xml
    {
        static inline bool isWhitespace(uint32_t c)
        {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n';
        }

        static inline bool isNameStartChar(uint32_t c)
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

        static inline bool isNameChar(uint32_t c)
        {
            return isNameStartChar(c) ||
                c == '-' || c == '.' ||
                (c >= '0' && c <= '9') ||
                c == 0xB7 ||
                (c >= 0x0300 && c <= 0x036F) ||
                (c >= 0x203F && c <= 0x2040);
        }

        static void skipWhitespaces(const std::vector<uint32_t>& str,
                                    std::vector<uint32_t>::const_iterator& iterator)
        {
            for (;;)
            {
                if (iterator == str.end()) break;

                if (isWhitespace(*iterator))
                    ++iterator;
                else
                    break;
            }
        }

        static std::string parseName(const std::vector<uint32_t>& str,
                                     std::vector<uint32_t>::const_iterator& iterator)
        {
            std::string result;

            if (iterator == str.end())
                throw std::runtime_error("Unexpected end of data");

            if (!isNameStartChar(*iterator))
                throw std::runtime_error("Invalid name start");

            for (;;)
            {
                if (iterator == str.end())
                    throw std::runtime_error("Unexpected end of data");

                if (!isNameChar(*iterator))
                    break;
                else
                {
                    result += utf32ToUtf8(*iterator);
                    ++iterator;
                }
            }

            return result;
        }

        static std::string parseEntity(const std::vector<uint32_t>& str,
                                       std::vector<uint32_t>::const_iterator& iterator)
        {
            std::string result;

            if (iterator == str.end())
                throw std::runtime_error("Unexpected end of data");

            if (*iterator != '&')
                throw std::runtime_error("Expected an ampersand");

            std::string value;

            for (;;)
            {
                if (++iterator == str.end())
                    throw std::runtime_error("Unexpected end of data");

                if (*iterator == ';')
                {
                    ++iterator;
                    break;
                }
                else
                    value.push_back(static_cast<char>(*iterator));
            }

            if (value.empty())
                throw std::runtime_error("Invalid entity");

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
                    throw std::runtime_error("Invalid entity");

                uint32_t c = 0;

                if (value[1] == 'x') // hex value
                {
                    if (value.length() != 2 + 4)
                        throw std::runtime_error("Invalid entity");

                    for (size_t i = 0; i < 4; ++i)
                    {
                        uint8_t code = 0;

                        if (value[i + 2] >= '0' && value[i + 2] <= '9') code = static_cast<uint8_t>(value[i + 2]) - '0';
                        else if (value[i + 2] >= 'a' && value[i + 2] <='f') code = static_cast<uint8_t>(value[i + 2]) - 'a' + 10;
                        else if (value[i + 2] >= 'A' && value[i + 2] <='F') code = static_cast<uint8_t>(value[i + 2]) - 'A' + 10;
                        else
                            throw std::runtime_error("Invalid character code");

                        c = (c << 4) | code;
                    }
                }
                else
                {
                    if (value.length() != 1 + 4)
                        throw std::runtime_error("Invalid entity");

                    for (size_t i = 0; i < 4; ++i)
                    {
                        uint8_t code = 0;

                        if (value[i + 1] >= '0' && value[i + 1] <= '9') code = static_cast<uint8_t>(value[i + 1]) - '0';
                        else
                            throw std::runtime_error("Invalid character code");

                        c = c * 10 + code;
                    }
                }

                result = utf32ToUtf8(c);
            }
            else
                throw std::runtime_error("Invalid entity");

            return result;
        }

        static std::string parseString(const std::vector<uint32_t>& str,
                                       std::vector<uint32_t>::const_iterator& iterator)
        {
            std::string result;

            if (iterator == str.end())
                throw std::runtime_error("Unexpected end of data");

            if (*iterator != '"' && *iterator != '\'')
                throw std::runtime_error("Expected quotes");

            char quotes = static_cast<char>(*iterator);

            ++iterator;

            for (;;)
            {
                if (iterator == str.end())
                    throw std::runtime_error("Unexpected end of data");

                if (*iterator == static_cast<uint32_t>(quotes))
                {
                    ++iterator;
                    break;
                }
                else if (*iterator == '&')
                {
                    std::string entity = parseEntity(str, iterator);
                    result += entity;
                }
                else
                {
                    result += utf32ToUtf8(*iterator);
                    ++iterator;
                }
            }

            return result;
        }

        static void encodeString(std::vector<uint8_t>& data,
                                 const std::vector<uint32_t>& str)
        {
            for (uint32_t c : str)
            {
                if (c == '"') data.insert(data.end(), {'&', 'q', 'u', 'o', 't', ';'});
                else if (c == '&') data.insert(data.end(), {'&', 'a', 'm', 'p', ';'});
                else if (c == '\'') data.insert(data.end(), {'&', 'a', 'p', 'o', 's', ';'});
                else if (c == '<') data.insert(data.end(), {'&', 'l', 't', ';'});
                else if (c == '>') data.insert(data.end(), {'&', 'g', 't', ';'});
                else
                {
                    std::string encoded = utf32ToUtf8(c);
                    data.insert(data.end(), encoded.begin(), encoded.end());
                }
            }
        }

        void Node::parse(const std::vector<uint32_t>& str,
                         std::vector<uint32_t>::const_iterator& iterator,
                         bool preserveWhitespaces,
                         bool preserveComments,
                         bool preserveProcessingInstructions)
        {
            if (iterator == str.end())
                throw std::runtime_error("Unexpected end of data");

            if (*iterator == '<')
            {
                if (++iterator == str.end())
                    throw std::runtime_error("Unexpected end of data");

                if (*iterator == '!') // <!
                {
                    if (++iterator == str.end())
                        throw std::runtime_error("Unexpected end of data");

                    if (*iterator == '-') // <!-
                    {
                        if (++iterator == str.end())
                            throw std::runtime_error("Unexpected end of data");

                        if (*iterator != '-') // <!--
                            throw std::runtime_error("Expected a comment");

                        for (;;)
                        {
                            if (std::distance<std::vector<uint32_t>::const_iterator>(++iterator, str.end()) < 3)
                                throw std::runtime_error("Unexpected end of data");

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
                                        throw std::runtime_error("Unexpected double-hyphen inside comment");
                                }
                            }

                            value += utf32ToUtf8(*iterator);
                        }

                        type = Type::COMMENT;
                    }
                    else if (*iterator == '[') // <![
                    {
                        ++iterator;
                        std::string name;
                        name = parseName(str, iterator);

                        if (name != "CDATA")
                            throw std::runtime_error("Expected CDATA");

                        if (iterator == str.end())
                            throw std::runtime_error("Unexpected end of data");

                        if (*iterator != '[')
                            throw std::runtime_error("Expected a left bracket");

                        for (;;)
                        {
                            if (std::distance<std::vector<uint32_t>::const_iterator>(++iterator, str.end()) < 3)
                                throw std::runtime_error("Unexpected end of data");

                            if (*iterator == ']' &&
                                *(iterator + 1) == ']' &&
                                *(iterator + 2) == '>')
                            {
                                iterator += 3;
                                break;
                            }

                            value += utf32ToUtf8(*iterator);
                        }

                        type = Type::CDATA;
                    }
                    else
                        throw std::runtime_error("Type declarations are not supported");
                }
                else if (*iterator == '?') // <?
                {
                    ++iterator;
                    value = parseName(str, iterator);

                    for (;;)
                    {
                        skipWhitespaces(str, iterator);

                        if (iterator == str.end())
                            throw std::runtime_error("Unexpected end of data");

                        if (*iterator == '?')
                        {
                            if (++iterator == str.end())
                                throw std::runtime_error("Unexpected end of data");

                            if (*iterator != '>') // ?>
                                throw std::runtime_error("Expected a right angle bracket");

                            ++iterator;
                            break;
                        }

                        std::string attribute = parseName(str, iterator);

                        skipWhitespaces(str, iterator);

                        if (iterator == str.end())
                            throw std::runtime_error("Unexpected end of data");

                        if (*iterator != '=')
                            throw std::runtime_error("Expected an equal sign");

                        ++iterator;

                        skipWhitespaces(str, iterator);
                        attributes[attribute] = parseString(str, iterator);
                    }

                    type = Type::PROCESSING_INSTRUCTION;
                }
                else // <
                {
                    value = parseName(str, iterator);

                    bool tagClosed = false;

                    for (;;)
                    {
                        skipWhitespaces(str, iterator);

                        if (iterator == str.end())
                            throw std::runtime_error("Unexpected end of data");

                        if (*iterator == '>')
                        {
                            ++iterator;
                            break;
                        }
                        else if (*iterator == '/')
                        {
                            if (++iterator == str.end())
                                throw std::runtime_error("Unexpected end of data");

                            if (*iterator != '>') // />
                                throw std::runtime_error("Expected a right angle bracket");

                            tagClosed = true;
                            ++iterator;
                            break;
                        }

                        std::string attribute;
                        attribute = parseName(str, iterator);

                        skipWhitespaces(str, iterator);

                        if (iterator == str.end())
                            throw std::runtime_error("Unexpected end of data");

                        if (*iterator != '=')
                            throw std::runtime_error("Expected an equal sign");

                        ++iterator;

                        skipWhitespaces(str, iterator);

                        attributes[attribute] = parseString(str, iterator);
                    }

                    if (!tagClosed)
                    {
                        for (;;)
                        {
                            if (!preserveWhitespaces) skipWhitespaces(str, iterator);

                            if (iterator == str.end())
                                throw std::runtime_error("Unexpected end of data");

                            if (*iterator == '<' &&
                                iterator + 1 != str.end() &&
                                *(iterator + 1) == '/')
                            {
                                ++iterator; // skip the left angle bracket
                                ++iterator; // skip the slash

                                std::string tag = parseName(str, iterator);

                                if (tag != value)
                                    throw std::runtime_error("Tag not closed properly");

                                if (iterator == str.end())
                                    throw std::runtime_error("Unexpected end of data");

                                if (*iterator != '>')
                                    throw std::runtime_error("Expected a right angle bracket");

                                ++iterator;

                                break;
                            }
                            else
                            {
                                Node node;
                                node.parse(str, iterator, preserveWhitespaces, preserveComments, preserveProcessingInstructions);

                                if ((preserveComments || node.getType() != Type::COMMENT) &&
                                    (preserveProcessingInstructions || node.getType() != Type::PROCESSING_INSTRUCTION))
                                    children.push_back(node);
                            }
                        }
                    }

                    type = Type::TAG;
                }
            }
            else
            {
                for (;;)
                {
                    if (iterator == str.end() || // end of a file
                        *iterator == '<') // start of a tag
                        break;
                    else if (*iterator == '&')
                    {
                        std::string entity = parseEntity(str, iterator);
                        value += entity;
                    }
                    else
                    {
                        value += utf32ToUtf8(*iterator);
                        ++iterator;
                    }
                }

                type = Type::TEXT;
            }
        }

        void Node::encode(std::vector<uint8_t>& data) const
        {
            switch (type)
            {
                case Node::Type::COMMENT:
                    data.insert(data.end(), {'<', '!', '-', '-'});
                    data.insert(data.end(), value.begin(), value.end());
                    data.insert(data.end(), {'-', '-', '>'});
                    break;
                case Node::Type::CDATA:
                    data.insert(data.end(), {'<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['});
                    data.insert(data.end(), value.begin(), value.end());
                    data.insert(data.end(), {']', ']', '>'});
                    break;
                case Node::Type::TYPE_DECLARATION:
                    throw std::runtime_error("Type declarations are not supported");
                case Node::Type::PROCESSING_INSTRUCTION:
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
                            encodeString(data, utf8ToUtf32(attribute.second));
                            data.insert(data.end(), '"');
                        }
                    }

                    data.insert(data.end(), {'?', '>'});
                    break;
                case Node::Type::TAG:
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
                case Node::Type::TEXT:
                    encodeString(data, utf8ToUtf32(value));
                    break;
                default:
                    throw std::runtime_error("Unknown node type");
            }

            for (const Node& node : children)
                node.encode(data);
        }

        Data::Data()
        {
        }

        Data::Data(const std::vector<uint8_t>& data,
                   bool preserveWhitespaces,
                   bool preserveComments,
                   bool preserveProcessingInstructions)
        {
            std::vector<uint32_t> str;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                bom = true;
                str = utf8ToUtf32(std::vector<uint8_t>(data.begin() + 3, data.end()));
            }
            else
            {
                bom = false;
                str = utf8ToUtf32(data);
            }

            bool rootTagFound = false;

            auto iterator = str.cbegin();

            for (;;)
            {
                if (!preserveWhitespaces) skipWhitespaces(str, iterator);

                if (iterator == str.end()) break;

                Node node;
                node.parse(str, iterator,
                           preserveWhitespaces,
                           preserveComments,
                           preserveProcessingInstructions);

                if ((preserveComments || node.getType() != Node::Type::COMMENT) &&
                    (preserveProcessingInstructions || node.getType() != Node::Type::PROCESSING_INSTRUCTION))
                {
                    children.push_back(node);

                    if (node.getType() == Node::Type::TAG)
                    {
                        if (rootTagFound)
                            throw std::runtime_error("Multiple root tags found");
                        else
                            rootTagFound = true;
                    }
                }
            }

            if (!rootTagFound)
                throw std::runtime_error("No root tag found");
        }

        std::vector<uint8_t> Data::encode() const
        {
            std::vector<uint8_t> result;

            if (bom) result = {0xEF, 0xBB, 0xBF};

            for (const Node& node : children)
                node.encode(result);

            return result;
        }
    } // namespace xml
} // namespace ouzel
