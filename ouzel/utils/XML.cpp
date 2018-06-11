// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "XML.hpp"
#include "core/Engine.hpp"
#include "Log.hpp"
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

        static bool skipWhitespaces(const std::vector<uint32_t>& str,
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

            return true;
        }

        static bool parseName(const std::vector<uint32_t>& str,
                              std::vector<uint32_t>::const_iterator& iterator,
                              std::string& result)
        {
            result.clear();

            if (iterator == str.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

            if (!isNameStartChar(*iterator))
            {
                Log(Log::Level::ERR) << "Invalid name start";
                return false;
            }

            for (;;)
            {
                if (iterator == str.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                if (!isNameChar(*iterator))
                    break;
                else
                {
                    result += utf32ToUtf8(*iterator);
                    ++iterator;
                }
            }

            return true;
        }

        static bool parseEntity(const std::vector<uint32_t>& str,
                                std::vector<uint32_t>::const_iterator& iterator,
                                std::string& result)
        {
            result.clear();

            if (iterator == str.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

            if (*iterator != '&')
            {
                Log(Log::Level::ERR) << "Expected an ampersand";
                return false;
            }

            std::string value;

            for (;;)
            {
                if (++iterator == str.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                if (*iterator == ';')
                {
                    ++iterator;
                    break;
                }
                else
                    value.push_back(static_cast<char>(*iterator));
            }

            if (value.empty())
            {
                Log(Log::Level::ERR) << "Invalid entity";
                return false;
            }

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
                {
                    Log(Log::Level::ERR) << "Invalid entity";
                    return false;
                }

                uint32_t c = 0;

                if (value[1] == 'x') // hex value
                {
                    if (value.length() != 2 + 4)
                    {
                        Log(Log::Level::ERR) << "Invalid entity";
                        return false;
                    }

                    for (size_t i = 0; i < 4; ++i)
                    {
                        uint8_t code = 0;

                        if (value[i + 2] >= '0' && value[i + 2] <= '9') code = static_cast<uint8_t>(value[i + 2]) - '0';
                        else if (value[i + 2] >= 'a' && value[i + 2] <='f') code = static_cast<uint8_t>(value[i + 2]) - 'a' + 10;
                        else if (value[i + 2] >= 'A' && value[i + 2] <='F') code = static_cast<uint8_t>(value[i + 2]) - 'A' + 10;
                        else
                        {
                            Log(Log::Level::ERR) << "Invalid character code";
                            return false;
                        }
                        c = (c << 4) | code;
                    }
                }
                else
                {
                    if (value.length() != 1 + 4)
                    {
                        Log(Log::Level::ERR) << "Invalid entity";
                        return false;
                    }

                    for (size_t i = 0; i < 4; ++i)
                    {
                        uint8_t code = 0;

                        if (value[i + 1] >= '0' && value[i + 1] <= '9') code = static_cast<uint8_t>(value[i + 1]) - '0';
                        else
                        {
                            Log(Log::Level::ERR) << "Invalid character code";
                            return false;
                        }
                        c = c * 10 + code;
                    }
                }

                result = utf32ToUtf8(c);
            }
            else
            {
                Log(Log::Level::ERR) << "Invalid entity";
                return false;
            }

            return true;
        }

        static bool parseString(const std::vector<uint32_t>& str,
                                std::vector<uint32_t>::const_iterator& iterator,
                                std::string& result)
        {
            result.clear();

            if (iterator == str.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

            if (*iterator != '"' && *iterator != '\'')
            {
                Log(Log::Level::ERR) << "Expected quotes";
                return false;
            }

            char quotes = static_cast<char>(*iterator);

            ++iterator;

            for (;;)
            {
                if (iterator == str.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                if (*iterator == static_cast<uint32_t>(quotes))
                {
                    ++iterator;
                    break;
                }
                else if (*iterator == '&')
                {
                    std::string entity;
                    if (!parseEntity(str, iterator, entity)) return false;
                    result += entity;
                }
                else
                {
                    result += utf32ToUtf8(*iterator);
                    ++iterator;
                }
            }

            return true;
        }

        static bool encodeString(std::vector<uint8_t>& data,
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

            return true;
        }

        bool Node::parse(const std::vector<uint32_t>& str,
                         std::vector<uint32_t>::const_iterator& iterator,
                         bool preserveWhitespaces,
                         bool preserveComments,
                         bool preserveProcessingInstructions)
        {
            if (iterator == str.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of data";
                return false;
            }

            if (*iterator == '<')
            {
                if (++iterator == str.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of data";
                    return false;
                }

                if (*iterator == '!') // <!
                {
                    if (++iterator == str.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of data";
                        return false;
                    }

                    if (*iterator == '-') // <!-
                    {
                        if (++iterator == str.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of data";
                            return false;
                        }

                        if (*iterator != '-') // <!--
                        {
                            Log(Log::Level::ERR) << "Expected a comment";
                            return false;
                        }

                        for (;;)
                        {
                            if (std::distance<std::vector<uint32_t>::const_iterator>(++iterator, str.end()) < 3)
                            {
                                Log(Log::Level::ERR) << "Unexpected end of data";
                                return false;
                            }

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
                                    {
                                        Log(Log::Level::ERR) << "Unexpected double-hyphen inside comment";
                                        return false;
                                    }
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
                        if (!parseName(str, iterator, name)) return false;

                        if (name != "CDATA")
                        {
                            Log(Log::Level::ERR) << "Expected CDATA";
                            return false;
                        }

                        if (iterator == str.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of data";
                            return false;
                        }

                        if (*iterator != '[')
                        {
                            Log(Log::Level::ERR) << "Expected a left bracket";
                            return false;
                        }

                        for (;;)
                        {
                            if (std::distance<std::vector<uint32_t>::const_iterator>(++iterator, str.end()) < 3)
                            {
                                Log(Log::Level::ERR) << "Unexpected end of data";
                                return false;
                            }

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
                    {
                        Log(Log::Level::ERR) << "Type declarations are not supported";
                        return false;
                    }
                }
                else if (*iterator == '?') // <?
                {
                    ++iterator;
                    if (!parseName(str, iterator, value)) return false;

                    for (;;)
                    {
                        if (!skipWhitespaces(str, iterator)) return false;

                        if (iterator == str.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of data";
                            return false;
                        }

                        if (*iterator == '?')
                        {
                            if (++iterator == str.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of data";
                                return false;
                            }

                            if (*iterator != '>') // ?>
                            {
                                Log(Log::Level::ERR) << "Expected a right angle bracket";
                                return false;
                            }

                            ++iterator;
                            break;
                        }

                        std::string attribute;
                        if (!parseName(str, iterator, attribute)) return false;

                        if (!skipWhitespaces(str, iterator)) return false;

                        if (iterator == str.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of data";
                            return false;
                        }

                        if (*iterator != '=')
                        {
                            Log(Log::Level::ERR) << "Expected an equal sign";
                            return false;
                        }

                        ++iterator;

                        if (!skipWhitespaces(str, iterator)) return false;

                        if (!parseString(str, iterator, attributes[attribute])) return false;
                    }

                    type = Type::PROCESSING_INSTRUCTION;

                    return true;
                }
                else // <
                {
                    if (!parseName(str, iterator, value)) return false;

                    bool tagClosed = false;

                    for (;;)
                    {
                        if (!skipWhitespaces(str, iterator)) return false;

                        if (iterator == str.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of data";
                            return false;
                        }

                        if (*iterator == '>')
                        {
                            ++iterator;
                            break;
                        }
                        else if (*iterator == '/')
                        {
                            if (++iterator == str.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of data";
                                return false;
                            }

                            if (*iterator != '>') // />
                            {
                                Log(Log::Level::ERR) << "Expected a right angle bracket";
                                return false;
                            }

                            tagClosed = true;
                            ++iterator;
                            break;
                        }

                        std::string attribute;
                        if (!parseName(str, iterator, attribute)) return false;

                        if (!skipWhitespaces(str, iterator)) return false;

                        if (iterator == str.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of data";
                            return false;
                        }

                        if (*iterator != '=')
                        {
                            Log(Log::Level::ERR) << "Expected an equal sign";
                            return false;
                        }

                        ++iterator;

                        if (!skipWhitespaces(str, iterator)) return false;

                        if (!parseString(str, iterator, attributes[attribute])) return false;
                    }

                    if (!tagClosed)
                    {
                        for (;;)
                        {
                            if (!preserveWhitespaces) skipWhitespaces(str, iterator);

                            if (iterator == str.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of data";
                                return false;
                            }

                            if (*iterator == '<' &&
                                iterator + 1 != str.end() &&
                                *(iterator + 1) == '/')
                            {
                                ++iterator; // skip the left angle bracket
                                ++iterator; // skip the slash

                                std::string tag;
                                if (!parseName(str, iterator, tag)) return false;

                                if (tag != value)
                                {
                                    Log(Log::Level::ERR) << "Tag not closed properly";
                                    return false;
                                }

                                if (iterator == str.end())
                                {
                                    Log(Log::Level::ERR) << "Unexpected end of data";
                                    return false;
                                }

                                if (*iterator != '>')
                                {
                                    Log(Log::Level::ERR) << "Expected a right angle bracket";
                                    return false;
                                }

                                ++iterator;

                                break;
                            }
                            else
                            {
                                Node node;
                                if (!node.parse(str, iterator, preserveWhitespaces, preserveComments, preserveProcessingInstructions))
                                    return false;

                                if ((preserveComments || node.getType() != Type::COMMENT) &&
                                    (preserveProcessingInstructions || node.getType() != Type::PROCESSING_INSTRUCTION))
                                    children.push_back(node);
                            }
                        }
                    }

                    type = Type::TAG;

                    return true;
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
                        std::string entity;
                        if (!parseEntity(str, iterator, entity)) return false;
                        value += entity;
                    }
                    else
                    {
                        value += utf32ToUtf8(*iterator);
                        ++iterator;
                    }
                }

                type = Type::TEXT;

                return true;
            }

            return true;
        }

        bool Node::encode(std::vector<uint8_t>& data) const
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
                    Log(Log::Level::ERR) << "Type declarations are not supported";
                    break;
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
                            if (!encodeString(data, utf8ToUtf32(attribute.second))) return false;
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
                    if (!encodeString(data, utf8ToUtf32(value))) return false;
                    break;
                default:
                    return false;
            }

            for (const Node& node : children)
                node.encode(data);

            return true;
        }

        Data::Data()
        {
        }

        Data::Data(const std::string& filename,
                   bool preserveWhitespaces,
                   bool preserveComments,
                   bool preserveProcessingInstructions)
        {
            init(filename,
                 preserveWhitespaces,
                 preserveComments,
                 preserveProcessingInstructions);
        }

        Data::Data(const std::vector<uint8_t>& data,
                   bool preserveWhitespaces,
                   bool preserveComments,
                   bool preserveProcessingInstructions)
        {
            init(data,
                 preserveWhitespaces,
                 preserveComments,
                 preserveProcessingInstructions);
        }

        bool Data::init(const std::string& filename,
                        bool preserveWhitespaces,
                        bool preserveComments,
                        bool preserveProcessingInstructions)
        {
            std::vector<uint8_t> data;

            if (!engine->getFileSystem()->readFile(filename, data))
                return false;

            return init(data,
                        preserveWhitespaces,
                        preserveComments,
                        preserveProcessingInstructions);
        }

        bool Data::init(const std::vector<uint8_t>& data,
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

            std::vector<uint32_t>::const_iterator iterator = str.cbegin();

            for (;;)
            {
                if (!preserveWhitespaces) skipWhitespaces(str, iterator);

                if (iterator == str.end()) break;

                Node node;
                if (!node.parse(str, iterator,
                                preserveWhitespaces,
                                preserveComments,
                                preserveProcessingInstructions))
                    return false;

                if ((preserveComments || node.getType() != Node::Type::COMMENT) &&
                    (preserveProcessingInstructions || node.getType() != Node::Type::PROCESSING_INSTRUCTION))
                {
                    children.push_back(node);

                    if (node.getType() == Node::Type::TAG)
                    {
                        if (rootTagFound)
                        {
                            Log(Log::Level::ERR) << "Multiple root tags found";
                            return false;
                        }
                        else
                            rootTagFound = true;
                    }
                }
            }

            if (!rootTagFound)
            {
                Log(Log::Level::ERR) << "No root tag found";
                return false;
            }

            return true;
        }

        bool Data::save(const std::string& filename) const
        {
            std::vector<uint8_t> data;

            if (!encode(data)) return false;

            return engine->getFileSystem()->writeFile(filename, data);
        }

        bool Data::encode(std::vector<uint8_t>& data) const
        {
            data.clear();

            if (bom) data.insert(data.end(), {0xEF, 0xBB, 0xBF});

            for (const Node& node : children)
                if (!node.encode(data)) return false;

            return true;
        }
    } // namespace xml
} // namespace ouzel
