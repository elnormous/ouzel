// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
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
        if (!sharedApplication->getFileSystem()->readFile(filename, data))
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

        int16_t k;
        uint32_t first, second;
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

                        texture = value;
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

                chars.insert(std::unordered_map<int32_t, CharDescriptor>::value_type(charId, c));
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
                k = 0;
                first = second = 0;
                while (!lineStream.eof())
                {
                    lineStream >> read;
                    i = read.find('=');
                    key = read.substr(0, i);
                    value = read.substr(i + 1);

                    //assign the correct value
                    std::stringstream converter;
                    converter << value;
                    if (key == "first") converter >> first;
                    else if (key == "second") converter >> second;
                    else if (key == "amount") converter >> k;
                }
                kern[std::make_pair(first, second)] = k;
            }
        }

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
        float total = 0.0f;

        std::vector<uint32_t> utf32Text = utf8to32(text);

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

    void BMFont::getVertices(const std::string& text,
                             const Color& color,
                             const Vector2& anchor,
                             const Vector2& scale,
                             std::vector<uint16_t>& indices,
                             std::vector<graphics::VertexPCT>& vertices)
    {
        Vector2 position;

        std::vector<uint32_t> utf32Text = utf8to32(text);

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

                textCoords[0] = Vector2(leftTop.v[0], leftTop.v[1]);
                textCoords[1] = Vector2(rightBottom.v[0], leftTop.v[1]);
                textCoords[2] = Vector2(leftTop.v[0], rightBottom.v[1]);
                textCoords[3] = Vector2(rightBottom.v[0], rightBottom.v[1]);

                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset, -position.v[1] - f.yOffset, 0.0f),
                                             color, textCoords[0]));

                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset, 0.0f),
                                             color, textCoords[1]));

                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset, -position.v[1] - f.yOffset - f.height, 0.0f),
                                             color, textCoords[2]));

                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset - f.height, 0.0f),
                                             color, textCoords[3]));

                if ((i + 1) != utf32Text.end())
                {
                    position.v[0] += static_cast<float>(getKerningPair(*i, *(i + 1)));
                }

                position.v[0] += f.xAdvance;
            }

            if (*i == static_cast<uint32_t>('\n') || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                float lineWidth = position.v[0];
                position.v[0] = 0.0f;
                position.v[1] += lineHeight;

                for (size_t c = firstChar; c < vertices.size(); ++c)
                {
                    vertices[c].position.v[0] -= lineWidth * anchor.v[0];
                }

                firstChar = vertices.size();
            }
        }

        float textHeight = position.v[1];

        for (size_t c = 0; c < vertices.size(); ++c)
        {
            vertices[c].position.v[1] += textHeight * (1.0f - anchor.v[1]);

            vertices[c].position.v[0] *= scale.v[0];
            vertices[c].position.v[1] *= scale.v[1];
        }
    }
}
