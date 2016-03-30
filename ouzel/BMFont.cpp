// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "BMFont.h"
#include "Engine.h"
#include "FileSystem.h"
#include "Vertex.h"
#include "Cache.h"
#include "Utils.h"

namespace ouzel
{
    BMFont::BMFont()
    {

    }

    BMFont::~BMFont()
    {

    }

    bool BMFont::parseFont(const std::string& filename)
    {
        std::ifstream stream(sharedEngine->getFileSystem()->getPath(filename));
        std::string line;
        std::string read, key, value;
        std::size_t i;

        if (!stream)
        {
            log("Failed to open font file");
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

                        _texture = sharedEngine->getCache()->getTexture(value, false, true);
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
                    if (key == "lineHeight") converter >> _lineHeight;
                    else if (key == "base") converter >> _base;
                    else if (key == "scaleW") converter >> _width;
                    else if (key == "scaleH") converter >> _height;
                    else if (key == "pages") converter >> _pages;
                    else if (key == "outline") converter >> _outline;
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

                _chars.insert(std::map<int32_t, CharDescriptor>::value_type(charId, c));
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
                    if (key == "count") converter >> _kernCount;
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
                _kern[std::make_pair(k.first, k.second)] = k;
            }
        }

        stream.close();

        return true;
    }

    int32_t BMFont::getKerningPair(int32_t first, int32_t second)
    {
        std::map<std::pair<int32_t, int32_t>, KerningInfo>::iterator i = _kern.find(std::make_pair(first, second));

        if (i != _kern.end())
        {
            return i->second.amount;
        }

        return 0;
    }

    float BMFont::getStringWidth(const std::string& text)
    {
        float total = 0;
        CharDescriptor* f;

        for (uint32_t i = 0; i < static_cast<uint32_t>(text.length()); i++)
        {
            std::map<int32_t, CharDescriptor>::iterator iter = _chars.find(text[i]);

            if (iter == _chars.end())
            {
                continue;
            }

            f = &iter->second;
            total += f->xAdvance;
        }

        return total;
    }

    bool BMFont::loadFont(const std::string& filename)
    {
        if (!parseFont(filename))
        {
            log("Failed to parse font %s",filename.c_str());
            return false;
        }

        if (!_texture)
        {
            return false;
        }

        _kernCount = static_cast<uint16_t>(_kern.size());

        return true;
    }

    void BMFont::getVertices(const std::string& text, const video::Color& color, const Vector2& anchor, std::vector<uint16_t>& indices, std::vector<video::VertexPCT>& vertices)
    {
        uint32_t flen;

        CharDescriptor  *f;

        float x = 0.0f;
        float y = _lineHeight * (1.0f - anchor.y);

        flen = static_cast<uint32_t>(text.length());

        indices.clear();
        vertices.clear();

        indices.reserve(flen * 6);
        vertices.reserve(flen * 4);

        Vector2 textCoords[4];

        for (uint32_t i = 0; i != flen; ++i)
        {
            std::map<int32_t, CharDescriptor>::iterator iter = _chars.find(text[i]);

            if (iter == _chars.end())
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

            Vector2 leftTop(f->x / static_cast<float>(_width),
                            f->y / static_cast<float>(_height));

            Vector2 rightBottom((f->x + f->width) / static_cast<float>(_width),
                                (f->y + f->height) / static_cast<float>(_height));

            textCoords[0] = Vector2(leftTop.x, leftTop.y);
            textCoords[1] = Vector2(rightBottom.x, leftTop.y);
            textCoords[2] = Vector2(leftTop.x, rightBottom.y);
            textCoords[3] = Vector2(rightBottom.x, rightBottom.y);

            vertices.push_back(video::VertexPCT(Vector3(x + f->xOffset, y - f->yOffset, 0.0f),
                                         color, textCoords[0]));

            vertices.push_back(video::VertexPCT(Vector3(x + f->xOffset + f->width, y - f->yOffset, 0.0f),
                                         color, textCoords[1]));

            vertices.push_back(video::VertexPCT(Vector3(x + f->xOffset, y - f->yOffset - f->height, 0.0f),
                                         color, textCoords[2]));

            vertices.push_back(video::VertexPCT(Vector3(x + f->xOffset + f->width, y - f->yOffset - f->height, 0.0f),
                                         color, textCoords[3]));

            // Only check kerning if there is greater then 1 character and
            // if the check character is 1 less then the end of the string.
            if (flen > 1 && i < flen)
            {
                x += getKerningPair(text[i], text[i+1]);
            }

            x +=  f->xAdvance;
        }

        float width = x;

        for (video::VertexPCT& vertex : vertices)
        {
            vertex.position.x -= width * anchor.x;
        }
    }
}
