// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "XML.hpp"
#include "core/Engine.hpp"
#include "Log.hpp"
#include "Utils.hpp"

static const std::vector<uint32_t> commentStart = {0x3c, 0x21, 0x2d, 0x2d}; // <!--
static const std::vector<uint32_t> commentEnd = {0x2d, 0x2d, 0x3e}; // -->

static const std::vector<uint32_t> cDataStart = {0x3c, 0x21, 0x5b, 0x43, 0x44, 0x41, 0x54, 0x41, 0x5b}; // <![CDATA[
static const std::vector<uint32_t> cDataEnd = {0x5d, 0x5d, 0x3e}; // ]]>

static const std::vector<uint32_t> doctypeStart = {0x3c, 0x21}; // <!
static const std::vector<uint32_t> doctypeEnd = {0x3e}; // >
static const std::vector<uint32_t> doctypeListStart = {0x5b}; // [
static const std::vector<uint32_t> doctypeListEnd = {0x5d}; // ]

static const std::vector<uint32_t> processingInstructionStart = {0x3c, 0x3f}; // <?
static const std::vector<uint32_t> processingInstructionEnd = {0x3f, 0x3e}; // ?>

static const std::vector<uint32_t> tagStart = {0x3c}; // <
static const std::vector<uint32_t> tagEnd = {0x3e}; // >

namespace ouzel
{
    namespace xml
    {
        Node::Node()
        {
        }

        bool Node::parseNode(const std::vector<uint32_t>& utf32,
                             std::vector<uint32_t>::iterator& iterator)
        {
            if (iterator == utf32.end())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (*iterator == '<')
            {
                if (iterator + 1 == utf32.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (*(iterator + 1) == '!') // <!
                {
                    if (iterator + 2 == utf32.end())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of file";
                        return false;
                    }

                    if (*(iterator + 2) == '-') // <!-
                    {
                        return parseComment(utf32, iterator);
                    }
                    else if (*(iterator + 2) == '[') // <![
                    {
                        return parseCData(utf32, iterator);
                    }
                    else
                    {
                        return parseDoctypeDefinition(utf32, iterator);
                    }
                }
                else if (*(iterator + 1) == '?') // <?
                {
                    return parseProcessingInstruction(utf32, iterator);
                }
                else // <
                {
                    return parseTag(utf32, iterator);
                }
            }
            else
            {
                Log(Log::Level::ERR) << "Expected start of a tag";
                return false;
            }

            return true;
        }

        bool Node::parseComment(const std::vector<uint32_t>& utf32,
                                std::vector<uint32_t>::iterator& iterator)
        {
            if (utf32.end() - iterator < commentStart.end() - commentStart.begin())
            {
                Log(Log::Level::ERR) << "Unexpected end of file";
                return false;
            }

            if (!std::equal(commentStart.begin(), commentStart.end(), iterator))
            {
                Log(Log::Level::ERR) << "Expected start of a comment";
                return false;
            }

            iterator += commentStart.end() - commentStart.begin();

            for (;;)
            {
                if (iterator == utf32.end())
                {
                    Log(Log::Level::ERR) << "Unexpected end of file";
                    return false;
                }

                if (*iterator == '-' &&
                    iterator + 1 != utf32.end() &&
                    *(iterator + 1) == '-')
                {
                    if (utf32.end() - iterator < commentEnd.end() - commentEnd.begin())
                    {
                        Log(Log::Level::ERR) << "Unexpected end of file";
                        return false;
                    }

                    if (!std::equal(commentEnd.begin(), commentEnd.end(), iterator))
                    {
                        Log(Log::Level::ERR) << "Expected end of a comment";
                        return false;
                    }

                    break;
                }
                else
                {
                    value += utf32to8(*iterator);
                    ++iterator;
                }
            }

            type = Type::COMMENT;
            return true;
        }

        bool Node::parseCData(const std::vector<uint32_t>& utf32,
                              std::vector<uint32_t>::iterator& iterator)
        {
            // TODO: parse CDATA <![CDATA[

            type = Type::CDATA;
            return true;
        }

        bool Node::parseDoctypeDefinition(const std::vector<uint32_t>& utf32,
                                          std::vector<uint32_t>::iterator& iterator)
        {
            // TODO: parse doctype (DTD) <!

            type = Type::DOCTYPE_DEFINITION;
            return true;
        }

        bool Node::parseProcessingInstruction(const std::vector<uint32_t>& utf32,
                                              std::vector<uint32_t>::iterator& iterator)
        {
            // TODO: parse processing instruction <?

            type = Type::PROCESSING_INSTRUCTION;
            return true;
        }

        bool Node::parseTag(const std::vector<uint32_t>& utf32,
                            std::vector<uint32_t>::iterator& iterator)
        {
            // TODO: parse until whitespace
            type = Type::TAG;
            return true;
        }

        Data::Data()
        {
        }

        Data::Data(const std::string& filename)
        {
            init(filename);
        }

        Data::Data(const std::vector<uint8_t>& data)
        {
            init(data);
        }

        bool Data::init(const std::string& filename)
        {
            std::vector<uint8_t> data;

            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data);
        }

        bool Data::init(const std::vector<uint8_t>& data)
        {
            std::vector<uint32_t> utf32;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                bom = true;
                std::vector<uint8_t> newData(data.begin() + 3, data.end());
                utf32 = utf8to32(newData);
            }
            else
            {
                bom = false;
                utf32 = utf8to32(data);
            }

            std::vector<uint32_t>::iterator iterator = utf32.begin();

            // TODO: parse xml declaration <?xml ... ?>

            return parseNode(utf32, iterator);
        }
    } // namespace xml
}
