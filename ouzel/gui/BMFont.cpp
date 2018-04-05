// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <string>
#include <iterator>
#include "BMFont.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "assets/Cache.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    static inline bool isWhitespace(uint8_t c)
    {
        return c == ' ' || c == '\t';
    }

    static inline bool isNewline(uint8_t c)
    {
        return c == '\r' || c == '\n';
    }

    static inline bool isControlChar(uint8_t c)
    {
        return c <= 0x1F;
    }

    static bool skipWhitespaces(const std::vector<uint8_t>& str,
                                std::vector<uint8_t>::const_iterator& iterator)
    {
        if (iterator == str.end()) return false;

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

    static void skipLine(const std::vector<uint8_t>& str,
                         std::vector<uint8_t>::const_iterator& iterator)
    {
        for (;;)
        {
            if (iterator == str.end()) break;

            if (isNewline(*iterator))
            {
                ++iterator;
                break;
            }

            ++iterator;
        }
    }

    static bool parseString(const std::vector<uint8_t>& str,
                            std::vector<uint8_t>::const_iterator& iterator,
                            std::string& result)
    {
        result.clear();

        if (*iterator == '"')
        {
            ++iterator;

            for (;;)
            {
                if (*iterator == '"' &&
                    (iterator + 1 == str.end() ||
                     isWhitespace(*(iterator + 1)) ||
                     isNewline(*(iterator + 1))))
                {
                    ++iterator;
                    break;
                }
                if (iterator == str.end()) return false;

                result.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            return true;
        }
        else
        {
            for (;;)
            {
                if (iterator == str.end() || isControlChar(*iterator) || isWhitespace(*iterator) || *iterator == '=') break;

                result.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            return !result.empty();
        }
    }

    static bool parseInt(const std::vector<uint8_t>& str,
                         std::vector<uint8_t>::const_iterator& iterator,
                         std::string& result)
    {
        result.clear();
        uint32_t length = 1;

        if (iterator != str.end() && *iterator == '-')
        {
            result.push_back(static_cast<char>(*iterator));
            ++length;
            ++iterator;
        }

        for (;;)
        {
            if (iterator == str.end() || *iterator < '0' || *iterator > '9') break;

            result.push_back(static_cast<char>(*iterator));

            ++iterator;
        }

        if (result.length() < length) return false;

        return true;
    }

    static bool parseToken(const std::vector<uint8_t>& str,
                           std::vector<uint8_t>::const_iterator& iterator,
                           char token)
    {
        if (iterator == str.end() || *iterator != static_cast<uint8_t>(token)) return false;

        ++iterator;

        return true;
    }

    BMFont::BMFont()
    {
    }

    BMFont::BMFont(const std::string& filename, bool mipmaps)
    {
        init(filename, mipmaps);
    }

    bool BMFont::init(const std::string& filename, bool mipmaps)
    {
        std::vector<uint8_t> data;
        if (!engine->getFileSystem()->readFile(filename, data))
        {
            return false;
        }

        return init(data, mipmaps);
    }

    bool BMFont::init(const std::vector<uint8_t>& data, bool mipmaps)
    {
        std::vector<uint8_t>::const_iterator iterator = data.begin();

        std::string keyword;
        std::string key;
        std::string value;

        for (;;)
        {
            if (iterator == data.end()) break;

            if (isNewline(*iterator))
            {
                // skip empty lines
                ++iterator;
            }
            else
            {
                if (!skipWhitespaces(data, iterator) ||
                    !parseString(data, iterator, keyword))
                {
                    Log(Log::Level::ERR) << "Failed to parse keyword";
                    return false;
                }

                if (keyword == "page")
                {
                    for (;;)
                    {
                        if (iterator == data.end() || isNewline(*iterator)) break;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, key))
                        {
                            Log(Log::Level::ERR) << "Failed to parse page";
                            return false;
                        }

                        if (!parseToken(data, iterator, '=') ||
                            !parseString(data, iterator, value))
                        {
                            Log(Log::Level::ERR) << "Failed to parse page";
                            return false;
                        }

                        if (key == "file")
                        {
                            fontTexture = engine->getCache()->getTexture(value, mipmaps);
                        }
                    }
                }
                else if (keyword == "common")
                {
                    for (;;)
                    {
                        if (iterator == data.end() || isNewline(*iterator)) break;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, key))
                        {
                            Log(Log::Level::ERR) << "Failed to parse common";
                            return false;
                        }

                        if (!parseToken(data, iterator, '='))
                        {
                            Log(Log::Level::ERR) << "Failed to parse common";
                            return false;
                        }

                        if (key == "lineHeight")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse lineHeight";
                                return false;
                            }

                            lineHeight = static_cast<uint16_t>(std::stoi(value));
                        }
                        else if (key == "base")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse base";
                                return false;
                            }

                            base = static_cast<uint16_t>(std::stoi(value));
                        }
                        else if (key == "scaleW")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse scaleW";
                                return false;
                            }

                            width = static_cast<uint16_t>(std::stoi(value));
                        }
                        else if (key == "scaleH")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse scaleH";
                                return false;
                            }

                            height = static_cast<uint16_t>(std::stoi(value));
                        }
                        else if (key == "pages")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse pages";
                                return false;
                            }

                            pages = static_cast<uint16_t>(std::stoi(value));
                        }
                        else if (key == "outline")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse outline";
                                return false;
                            }

                            outline = static_cast<uint16_t>(std::stoi(value));
                        }
                        else
                        {
                            if (!parseString(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse common";
                                return false;
                            }
                        }
                    }
                }
                else if (keyword == "char")
                {
                    uint32_t charId = 0;
                    CharDescriptor c;

                    for (;;)
                    {
                        if (iterator == data.end() || isNewline(*iterator)) break;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, key))
                        {
                            Log(Log::Level::ERR) << "Failed to parse char";
                            return false;
                        }

                        if (!parseToken(data, iterator, '='))
                        {
                            Log(Log::Level::ERR) << "Failed to parse char";
                            return false;
                        }

                        if (key == "id")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse id";
                                return false;
                            }

                            charId = static_cast<uint32_t>(std::stoul(value));
                        }
                        else if (key == "x")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse x";
                                return false;
                            }

                            c.x = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "y")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse y";
                                return false;
                            }

                            c.y = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "width")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse width";
                                return false;
                            }

                            c.width = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "height")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse height";
                                return false;
                            }

                            c.height = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "xoffset")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse xoffset";
                                return false;
                            }

                            c.xOffset = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "yoffset")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse yoffset";
                                return false;
                            }

                            c.yOffset = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "xadvance")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse xadvance";
                                return false;
                            }

                            c.xAdvance = static_cast<int16_t>(std::stoi(value));
                        }
                        else if (key == "page")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse page";
                                return false;
                            }

                            c.page = static_cast<int16_t>(std::stoi(value));
                        }
                        else
                        {
                            if (!parseString(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse char";
                                return false;
                            }
                        }
                    }

                    chars.insert(std::unordered_map<int32_t, CharDescriptor>::value_type(charId, c));
                }
                else if (keyword == "kernings")
                {
                    for (;;)
                    {
                        if (iterator == data.end() || isNewline(*iterator)) break;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, key))
                        {
                            Log(Log::Level::ERR) << "Failed to parse kernings";
                            return false;
                        }

                        if (!parseToken(data, iterator, '='))
                        {
                            Log(Log::Level::ERR) << "Failed to parse kernings";
                            return false;
                        }

                        if (key == "count")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse count";
                                return false;
                            }

                            kernCount = static_cast<uint16_t>(std::stoi(value));
                        }
                        else
                        {
                            if (!parseString(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse kernings";
                                return false;
                            }
                        }
                    }
                }
                else if (keyword == "kerning")
                {
                    int16_t amount = 0;
                    uint32_t first = 0;
                    uint32_t second = 0;

                    for (;;)
                    {
                        if (iterator == data.end() || isNewline(*iterator)) break;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, key))
                        {
                            Log(Log::Level::ERR) << "Failed to parse kerning";
                            return false;
                        }

                        if (!parseToken(data, iterator, '='))
                        {
                            Log(Log::Level::ERR) << "Failed to parse kerning";
                            return false;
                        }

                        if (key == "first")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse lineHeight";
                                return false;
                            }

                            first = static_cast<uint32_t>(std::stoul(value));
                        }
                        else if (key == "second")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse base";
                                return false;
                            }

                            second = static_cast<uint32_t>(std::stoul(value));
                        }
                        else if (key == "amount")
                        {
                            if (!parseInt(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse scaleW";
                                return false;
                            }

                            amount = static_cast<int16_t>(std::stoi(value));
                        }
                        else
                        {
                            if (!parseString(data, iterator, value))
                            {
                                Log(Log::Level::ERR) << "Failed to parse kerning";
                                return false;
                            }
                        }

                        kern[std::make_pair(first, second)] = amount;
                    }
                }
                else
                {
                    skipLine(data, iterator);
                }
            }
        }

        return true;
    }

    bool BMFont::getVertices(const std::string& text,
                             const Color& color,
                             float fontSize,
                             const Vector2& anchor,
                             std::vector<uint16_t>& indices,
                             std::vector<graphics::Vertex>& vertices,
                             std::shared_ptr<graphics::Texture>& texture)
    {
        Vector2 position;

        std::vector<uint32_t> utf32Text = utf8ToUtf32(text);

        indices.clear();
        vertices.clear();

        indices.reserve(utf32Text.size() * 6);
        vertices.reserve(utf32Text.size() * 4);

        Vector2 textCoords[4];

        size_t firstChar = 0;

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            std::unordered_map<uint32_t, CharDescriptor>::iterator iter = chars.find(*i);

            if (iter != chars.end())
            {
                const CharDescriptor& f = iter->second;

                uint16_t startIndex = static_cast<uint16_t>(vertices.size());
                indices.push_back(startIndex + 0);
                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 2);

                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 3);
                indices.push_back(startIndex + 2);

                Vector2 leftTop(f.x / static_cast<float>(width),
                                f.y / static_cast<float>(height));

                Vector2 rightBottom((f.x + f.width) / static_cast<float>(width),
                                    (f.y + f.height) / static_cast<float>(height));

                textCoords[0] = Vector2(leftTop.x, rightBottom.y);
                textCoords[1] = Vector2(rightBottom.x, rightBottom.y);
                textCoords[2] = Vector2(leftTop.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, leftTop.y);

                vertices.push_back(graphics::Vertex(Vector3(position.x + f.xOffset, -position.y - f.yOffset - f.height, 0.0F),
                                                    color, textCoords[0], Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(position.x + f.xOffset + f.width, -position.y - f.yOffset - f.height, 0.0F),
                                                    color, textCoords[1], Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(position.x + f.xOffset, -position.y - f.yOffset, 0.0F),
                                                    color, textCoords[2], Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(position.x + f.xOffset + f.width, -position.y - f.yOffset, 0.0F),
                                                    color, textCoords[3], Vector3(0.0F, 0.0F, -1.0F)));

                if ((i + 1) != utf32Text.end())
                {
                    position.x += static_cast<float>(getKerningPair(*i, *(i + 1)));
                }

                position.x += f.xAdvance;
            }

            if (*i == static_cast<uint32_t>('\n') || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                float lineWidth = position.x;
                position.x = 0.0F;
                position.y += lineHeight;

                for (size_t c = firstChar; c < vertices.size(); ++c)
                {
                    vertices[c].position.x -= lineWidth * anchor.x;
                }

                firstChar = vertices.size();
            }
        }

        float textHeight = position.y;

        for (size_t c = 0; c < vertices.size(); ++c)
        {
            vertices[c].position.y += textHeight * (1.0F - anchor.y);

            vertices[c].position.x *= fontSize;
            vertices[c].position.y *= fontSize;
        }

        texture = fontTexture;

        return true;
    }

    int16_t BMFont::getKerningPair(uint32_t first, uint32_t second)
    {
        auto i = kern.find(std::make_pair(first, second));

        if (i != kern.end())
        {
            return i->second;
        }

        return 0;
    }

    float BMFont::getStringWidth(const std::string& text)
    {
        float total = 0.0F;

        std::vector<uint32_t> utf32Text = utf8ToUtf32(text);

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            std::unordered_map<uint32_t, CharDescriptor>::iterator iter = chars.find(*i);

            if (iter != chars.end())
            {
                const CharDescriptor& f = iter->second;
                total += f.xAdvance;
            }
        }

        return total;
    }
}
