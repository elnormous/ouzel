// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "BMFont.h"
#include "core/Engine.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "graphics/Vertex.h"
#include "core/Cache.h"
#include "utils/Utils.h"

namespace ouzel
{
    template<typename CharT, typename TraitsT = std::char_traits<CharT>>
    class VectorBuffer: public std::basic_streambuf<CharT, TraitsT>
    {
    public:
        VectorBuffer(std::vector<uint8_t>& vec)
        {
            std::basic_streambuf<CharT, TraitsT>::setg(reinterpret_cast<CharT*>(vec.data()),
                                                       reinterpret_cast<CharT*>(vec.data()),
                                                       reinterpret_cast<CharT*>(vec.data() + vec.size()));
        }
    };

    BMFont::BMFont()
    {
    }

    BMFont::BMFont(const std::string& filename)
    {
        if (!parseFont(filename))
        {
            log("Failed to parse font %s",filename.c_str());
        }

        kernCount = static_cast<uint16_t>(kern.size());
    }
    
    BMFont::~BMFont()
    {
    }

    bool BMFont::parseFont(const std::string& filename)
    {
        std::vector<uint8_t> data;
        if (!sharedApplication->getFileSystem()->loadFile(filename, data))
        {
            return false;
        }

        VectorBuffer<char> databuf(data);
        std::istream stream(&databuf);

        std::string line;
        std::string read, key, value;
        std::size_t i;

        if (!stream)
        {
            log("Failed to open font file %s", filename.c_str());
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
        std::map<std::pair<int32_t, int32_t>, KerningInfo>::iterator i = kern.find(std::make_pair(first, second));

        if (i != kern.end())
        {
            return i->second.amount;
        }

        return 0;
    }

    float BMFont::getStringWidth(const std::string& text)
    {
        float total = 0;
        CharDescriptor* f;

        for (uint32_t i = 0; i < static_cast<uint32_t>(text.length()); ++i)
        {
            std::map<int32_t, CharDescriptor>::iterator iter = chars.find(text[i]);

            if (iter == chars.end())
            {
                continue;
            }

            f = &iter->second;
            total += f->xAdvance;
        }

        return total;
    }

    void BMFont::getVertices(const std::string& text, const graphics::Color& color, const Vector2& anchor, std::vector<uint16_t>& indices, std::vector<graphics::VertexPCT>& vertices)
    {
        uint32_t flen;

        CharDescriptor  *f;

        float x = 0.0f;
        float y = lineHeight * (1.0f - anchor.y);

        flen = static_cast<uint32_t>(text.length());

        indices.clear();
        vertices.clear();

        indices.reserve(flen * 6);
        vertices.reserve(flen * 4);

        Vector2 textCoords[4];

        for (uint32_t i = 0; i != flen; ++i)
        {
            std::map<int32_t, CharDescriptor>::iterator iter = chars.find(text[i]);

            if (iter == chars.end())
            {
                continue;
            }
            else
            {
                f = &iter->second;
            }

            uint16_t startIndex = static_cast<uint16_t>(vertices.size());
            indices.push_back(startIndex + 0);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 3);
            indices.push_back(startIndex + 2);

            Vector2 leftTop(f->x / static_cast<float>(width),
                            f->y / static_cast<float>(height));

            Vector2 rightBottom((f->x + f->width) / static_cast<float>(width),
                                (f->y + f->height) / static_cast<float>(height));

            textCoords[0] = Vector2(leftTop.x, leftTop.y);
            textCoords[1] = Vector2(rightBottom.x, leftTop.y);
            textCoords[2] = Vector2(leftTop.x, rightBottom.y);
            textCoords[3] = Vector2(rightBottom.x, rightBottom.y);

            if (texture->isFlipped())
            {
                leftTop.y = 1.0f - leftTop.y;
                rightBottom.y = 1.0f - rightBottom.y;
            }

            vertices.push_back(graphics::VertexPCT(Vector3(x + f->xOffset, y - f->yOffset, 0.0f),
                                         color, textCoords[0]));

            vertices.push_back(graphics::VertexPCT(Vector3(x + f->xOffset + f->width, y - f->yOffset, 0.0f),
                                         color, textCoords[1]));

            vertices.push_back(graphics::VertexPCT(Vector3(x + f->xOffset, y - f->yOffset - f->height, 0.0f),
                                         color, textCoords[2]));

            vertices.push_back(graphics::VertexPCT(Vector3(x + f->xOffset + f->width, y - f->yOffset - f->height, 0.0f),
                                         color, textCoords[3]));

            // Only check kerning if there is greater then 1 character and
            // if the check character is 1 less then the end of the string.
            if (flen > 1 && i < flen)
            {
                x += getKerningPair(text[i], text[i+1]);
            }

            x +=  f->xAdvance;
        }

        float totalWidth = x;

        for (graphics::VertexPCT& vertex : vertices)
        {
            vertex.position.x -= totalWidth * anchor.x;
        }
    }
}
