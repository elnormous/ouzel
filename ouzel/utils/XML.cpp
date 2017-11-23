// Copyright (C) 2017 Elviss Strazdins
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
        Node::Node()
        {
        }

        bool Node::parse(const std::vector<uint32_t>& utf32,
                         std::vector<uint32_t>::iterator& iterator,
                         bool preserveComments, bool preserveProcessingInstructions)
        {
            if (iterator == utf32.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (*iterator == '<')
            {
                ++iterator;
                
                if (iterator == utf32.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (*iterator == '!') // <!
                {
                    ++iterator;
                    
                    if (iterator == utf32.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of file";
                        return false;
                    }

                    if (*iterator == '-') // <!-
                    {
                        ++iterator;

                        if (iterator == utf32.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of file";
                            return false;
                        }

                        if (*iterator != '-') // <!--
                        {
                            Log(Log::Level::ERR) << "Expected a comment";
                            return false;
                        }

                        ++iterator;

                        for (;;)
                        {
                            if (iterator == utf32.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of file";
                                return false;
                            }

                            if (*iterator == '-')
                            {
                                if (iterator + 1 == utf32.end())
                                {
                                    Log(Log::Level::ERR) << "Unexpected end of file";
                                    return false;
                                }
                                
                                if (*(iterator + 1) == '-') // --
                                {
                                    iterator += 2;

                                    if (iterator == utf32.end())
                                    {
                                        Log(Log::Level::ERR) << "Unexpected end of file";
                                        return false;
                                    }
                                    
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

                            value += utf32to8(*iterator);
                            ++iterator;
                        }
                        
                        type = Type::COMMENT;
                    }
                    else if (*iterator == '[') // <![
                    {
                        return true;
                    }
                    else
                    {
                        return true;
                    }
                }
                else if (*iterator == '?') // <?
                {
                    ++iterator;

                    if (!parseName(utf32, iterator, value)) return false;

                    for (;;)
                    {
                        if (!skipWhitespaces(utf32, iterator)) return false;

                        if (iterator == utf32.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of file";
                            return false;
                        }

                        if (*iterator == '?')
                        {
                            ++iterator;

                            if (iterator == utf32.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of file";
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
                        if (!parseName(utf32, iterator, attribute)) return false;

                        if (!skipWhitespaces(utf32, iterator)) return false;

                        if (iterator == utf32.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of file";
                            return false;
                        }

                        if (*iterator != '=')
                        {
                            Log(Log::Level::ERR) << "Expected an equal sign";
                            return false;
                        }

                        ++iterator;
                        
                        if (!skipWhitespaces(utf32, iterator)) return false;
                        
                        if (!parseString(utf32, iterator, attributes[attribute])) return false;
                    }

                    type = Type::PROCESSING_INSTRUCTION;

                    return true;
                }
                else // <
                {
                    if (!parseName(utf32, iterator, value)) return false;

                    bool tagClosed = false;

                    for (;;)
                    {
                        if (!skipWhitespaces(utf32, iterator)) return false;

                        if (iterator == utf32.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of file";
                            return false;
                        }

                        if (*iterator == '>')
                        {
                            ++iterator;
                            break;
                        }
                        else if (*iterator == '/')
                        {
                            ++iterator;

                            if (iterator == utf32.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of file";
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
                        if (!parseName(utf32, iterator, attribute)) return false;

                        if (!skipWhitespaces(utf32, iterator)) return false;

                        if (iterator == utf32.end())
                        {
                            Log(Log::Level::ERR) << "Unexpected end of file";
                            return false;
                        }

                        if (*iterator != '=')
                        {
                            Log(Log::Level::ERR) << "Expected an equal sign";
                            return false;
                        }

                        ++iterator;

                        if (!skipWhitespaces(utf32, iterator)) return false;

                        if (!parseString(utf32, iterator, attributes[attribute])) return false;
                    }

                    if (!tagClosed)
                    {
                        for (;;)
                        {
                            if (iterator == utf32.end())
                            {
                                Log(Log::Level::ERR) << "Unexpected end of file";
                                return false;
                            }

                            if (*iterator == '<' && iterator + 1 != utf32.end() &&
                                *(iterator + 1) == '/')
                            {
                                ++iterator; // skip the left angle bracket
                                ++iterator; // skip the slash

                                std::string tag;
                                if (!parseName(utf32, iterator, tag)) return false;

                                if (tag != value)
                                {
                                    Log(Log::Level::ERR) << "Tag not closed properly";
                                    return false;
                                }

                                if (iterator == utf32.end())
                                {
                                    Log(Log::Level::ERR) << "Unexpected end of file";
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
                                if (!node.parse(utf32, iterator, preserveComments, preserveProcessingInstructions))
                                {
                                    return false;
                                }

                                if ((preserveComments || node.getType() != Type::COMMENT) &&
                                    (preserveProcessingInstructions || node.getType() != Type::PROCESSING_INSTRUCTION))
                                {
                                    children.push_back(node);
                                }
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
                    if (iterator == utf32.end() || // end of a file
                        *iterator == '<') // start of a tag
                    {
                        break;
                    }
                    else
                    {
                        value += utf32to8(*iterator);
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
                    break;
                case Node::Type::DOCTYPE_DEFINITION:
                    break;
                case Node::Type::PROCESSING_INSTRUCTION:
                    data.insert(data.end(), {'<', '?'});
                    data.insert(data.end(), value.begin(), value.end());

                    if (attributes.empty())
                    {
                        data.insert(data.end(), ' ');
                    }
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

                    data.insert(data.end(), {'?', '>'});
                    break;
                case Node::Type::TAG:
                    data.insert(data.end(), '<');
                    data.insert(data.end(), value.begin(), value.end());

                    if (attributes.empty())
                    {
                        data.insert(data.end(), ' ');
                    }
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
                    {
                        data.insert(data.end(), {'/', '>'});
                    }
                    else
                    {
                        data.insert(data.end(), '>');

                        for (const Node& node : children)
                        {
                            node.encode(data);
                        }

                        data.insert(data.end(), {'<', '/'});
                        data.insert(data.end(), value.begin(), value.end());
                        data.insert(data.end(), '>');
                    }
                    break;
                case Node::Type::TEXT:
                    data.insert(data.end(), value.begin(), value.end());
                    break;
                default:
                    return false;
            }

            for (const Node& node : children)
            {
                node.encode(data);
            }

            return true;
        }

        static bool isWhitespace(uint32_t c)
        {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n';
        }

        static bool isNameStartChar(uint32_t c)
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

        static bool isNameChar(uint32_t c)
        {
            return isNameStartChar(c) ||
                c == '-' || c == '.' ||
                (c >= '0' && c <= '9') ||
                c == 0xB7 ||
                (c >= 0x0300 && c <= 0x036F) ||
                (c >= 0x203F && c <= 0x2040);
        }

        bool Node::skipWhitespaces(const std::vector<uint32_t>& utf32,
                                   std::vector<uint32_t>::iterator& iterator)
        {
            for (;;)
            {
                if (iterator == utf32.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (isWhitespace(*iterator))
                {
                    ++iterator;
                }
                else
                {
                    break;
                }
            }

            return true;
        }

        bool Node::parseName(const std::vector<uint32_t>& utf32,
                             std::vector<uint32_t>::iterator& iterator,
                             std::string& result)
        {
            result.clear();

            if (iterator == utf32.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (!isNameStartChar(*iterator))
            {
                Log(Log::Level::ERR) << "Invalid name start";
                return false;
            }

            for (;;)
            {
                if (iterator == utf32.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (!isNameChar(*iterator))
                {
                    break;
                }
                else
                {
                    result += utf32to8(*iterator);
                    ++iterator;
                }
            }

            return true;
        }

        bool Node::parseString(const std::vector<uint32_t>& utf32,
                               std::vector<uint32_t>::iterator& iterator,
                               std::string& result)
        {
            result.clear();

            if (iterator == utf32.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
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
                if (iterator == utf32.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (quotes == static_cast<char>(*iterator))
                {
                    ++iterator;
                    break;
                }

                result += utf32to8(*iterator);
                ++iterator;
            }

            return true;
        }

        Data::Data()
        {
        }

        Data::Data(const std::string& filename, bool preserveComments, bool preserveProcessingInstructions)
        {
            init(filename, preserveComments, preserveProcessingInstructions);
        }

        Data::Data(const std::vector<uint8_t>& data, bool preserveComments, bool preserveProcessingInstructions)
        {
            init(data, preserveComments, preserveProcessingInstructions);
        }

        bool Data::init(const std::string& filename, bool preserveComments, bool preserveProcessingInstructions)
        {
            std::vector<uint8_t> data;

            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data, preserveComments, preserveProcessingInstructions);
        }

        bool Data::init(const std::vector<uint8_t>& data, bool preserveComments, bool preserveProcessingInstructions)
        {
            std::vector<uint32_t> utf32;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                bom = true;
                utf32 = utf8to32(std::vector<uint8_t>(data.begin() + 3, data.end()));
            }
            else
            {
                bom = false;
                utf32 = utf8to32(data);
            }

            bool rootTagFound = false;

            std::vector<uint32_t>::iterator iterator = utf32.begin();

            while (iterator != utf32.end())
            {
                Node node;
                if (!node.parse(utf32, iterator, preserveComments, preserveProcessingInstructions))
                {
                    return false;
                }

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
                        {
                            rootTagFound = true;
                        }
                    }
                }
            }

            if (!rootTagFound)
            {
                Log(Log::Level::ERR) << "No root tag found";
                return false;
            }

            // TODO: check if only single tag is in the children list

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
            {
                if (!node.encode(data)) return false;
            }

            return true;
        }
    } // namespace xml
}
