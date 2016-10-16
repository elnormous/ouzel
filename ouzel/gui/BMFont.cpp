// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iostream>
#include <string>
#include <sstream>
#include "utf8/unchecked.h"
#include "BMFont.h"
#include "core/Engine.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "graphics/Vertex.h"
#include "core/Cache.h"
#include "utils/Log.h"

namespace ouzel
{
    BMFont::BMFont()
    {
    }

    BMFont::BMFont(const std::string& filename)
    {
        if (!parseFont(filename))
        {
            Log(Log::Level::ERR) << "Failed to parse font " << filename;
        }

        kernCount = static_cast<uint16_t>(kern.size());
    }

    bool BMFont::parseFont(const std::string& filename)
    {
        std::vector<uint8_t> data;
        if (!sharedApplication->getFileSystem()->loadFile(filename, data))
        {
            return false;
        }

        std::stringstream stream;
        std::copy(data.begin(), data.end(), std::ostream_iterator<uint8_t>(stream));

        std::string line;
        std::string read, key, value;
        std::size_t i;

        if (!stream)
        {
            Log(Log::Level::ERR) << "Failed to open font file " << filename;
            return false;
        }

        KerningInfo k;
        CharDescriptor c;

        while (!stream.eof())
        {
            std::getline(stream, line);

            std::stringstream lineStream;
            lineStream << line;

            lineStream >> read;

            if (read == "page")
            {
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "file")
                    {
                        // trim quotes
                        if (value.length() && value[0] == '"' && value[value.length() - 1] == '"')
                        {
                            value = value.substr(1, value.length() - 2);
                        }

                        texture = sharedEngine->getCache()->getTexture(value, false, true);
                    }
                }
            }
            else if (read == "common")
            {
                //this holds common data
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "lineHeight") converter >> lineHeight;
                    else if (key == "base") converter >> base;
                    else if (key == "scaleW") converter >> width;
                    else if (key == "scaleH") converter >> height;
                    else if (key == "pages") converter >> pages;
                    else if (key == "outline") converter >> outline;
                }
            }
            else if (read == "char")
            {
                //This is data for each specific character.
                int32_t charId = 0;

                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //Assign the correct value
                    converter << value;
                    if (key == "id") converter >> charId;
                    else if (key == "x") converter >> c.x;
                    else if (key == "y") converter >> c.y;
                    else if (key == "width") converter >> c.width;
                    else if (key == "height") converter >> c.height;
                    else if (key == "xoffset") converter >> c.xOffset;
                    else if (key == "yoffset") converter >> c.yOffset;
                    else if (key == "xadvance") converter >> c.xAdvance;
                    else if (key == "page") converter >> c.page;
                }

                chars.insert(std::map<int32_t, CharDescriptor>::value_type(charId, c));
            }
            else if (read == "kernings")
            {
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "count") converter >> kernCount;
                }
            }
            else if (read == "kerning")
            {
                while (!lineStream.eof())
                {
                    std::stringstream converter;
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    converter << value;
                    if (key == "first") converter >> k.first;
                    else if (key == "second") converter >> k.second;
                    else if (key == "amount") converter >> k.amount;
                }
                kern[std::make_pair(k.first, k.second)] = k;
            }
        }

        return true;
    }

    int32_t BMFont::getKerningPair(int32_t first, int32_t second)
    {
        std::map<std::pair<uint32_t, uint32_t>, KerningInfo>::iterator i = kern.find(std::make_pair(first, second));

        if (i != kern.end())
        {
            return i->second.amount;
        }

        return 0;
    }

    float BMFont::getStringWidth(const std::string& text)
    {
        float total = 0.0f;

        for (auto i = text.begin(); i != text.end();)
        {
            uint32_t c = utf8::unchecked::next(i);

            std::map<uint32_t, CharDescriptor>::iterator iter = chars.find(c);

            if (iter != chars.end())
            {
                const CharDescriptor& f = iter->second;
                total += f.xAdvance;
            }
        }

        return total;
    }

    void BMFont::getVertices(const std::string& text, const graphics::Color& color, const Vector2& anchor, std::vector<uint16_t>& indices, std::vector<graphics::VertexPCT>& vertices)
    {
        Vector2 position;

        std::vector<uint32_t> utf32Text;
        utf8::unchecked::utf8to32(text.begin(), text.end(), std::back_inserter(utf32Text));

        indices.clear();
        vertices.clear();

        indices.reserve(utf32Text.size() * 6);
        vertices.reserve(utf32Text.size() * 4);

        Vector2 textCoords[4];

        size_t firstChar = 0;

        for (size_t i = 0; i < utf32Text.size(); ++i)
        {
            std::map<uint32_t, CharDescriptor>::iterator iter = chars.find(utf32Text[i]);

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

                if (texture->isFlipped())
                {
                    leftTop.y = 1.0f - leftTop.y;
                    rightBottom.y = 1.0f - rightBottom.y;
                }

                textCoords[0] = Vector2(leftTop.x, leftTop.y);
                textCoords[1] = Vector2(rightBottom.x, leftTop.y);
                textCoords[2] = Vector2(leftTop.x, rightBottom.y);
                textCoords[3] = Vector2(rightBottom.x, rightBottom.y);

                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset, -position.y - f.yOffset, 0.0f),
                                             color, textCoords[0]));

                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset + f.width, -position.y - f.yOffset, 0.0f),
                                             color, textCoords[1]));

                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset, -position.y - f.yOffset - f.height, 0.0f),
                                             color, textCoords[2]));

                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset + f.width, -position.y - f.yOffset - f.height, 0.0f),
                                             color, textCoords[3]));

                // Only check kerning if there is greater then 1 character and
                // if the check character is 1 less then the end of the string.
                if (utf32Text.size() > 1 && i < utf32Text.size() - 1)
                {
                    position.x += getKerningPair(text[i], text[i + 1]);
                }

                position.x +=  f.xAdvance;
            }

            if (utf32Text[i] == static_cast<uint32_t>('\n') || // line feed
                i + 1 == utf32Text.size()) // end of string
            {
                float lineWidth = position.x;
                position.x = 0.0f;
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
            vertices[c].position.y += textHeight * anchor.y;
        }
    }
}
