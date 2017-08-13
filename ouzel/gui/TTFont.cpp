// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>

#include "TTFont.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#include "../../external/stb/stb_truetype.h"

namespace ouzel
{
    TTFont::TTFont():
        Font()
    {
    }

    TTFont::TTFont(const std::string & filename, uint16_t pt, bool mipmaps, UTFChars flag)
    {
        if (!parseFont(filename, pt, mipmaps, flag))
        {
            Log(Log::Level::ERR) << "Failed to parse font " << filename;
        }

        kernCount = static_cast<uint16_t>(kern.size());
    }

    void TTFont::getVertices(const std::string& text,
                             const Color& color,
                             const Vector2& anchor,
                             const Vector2& scale,
                             std::vector<uint16_t>& indices,
                             std::vector<graphics::VertexPCT>& vertices,
                             std::shared_ptr<graphics::Texture>& texture)
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

                textCoords[0] = Vector2(leftTop.v[0], rightBottom.v[1]);
                textCoords[1] = Vector2(rightBottom.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2(leftTop.v[0], leftTop.v[1]);
                textCoords[3] = Vector2(rightBottom.v[0], leftTop.v[1]);

                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset, -position.v[1] - f.yOffset - f.height, 0.0f),
                                                       color, textCoords[0]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset - f.height, 0.0f),
                                                       color, textCoords[1]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset, -position.v[1] - f.yOffset, 0.0f),
                                                       color, textCoords[2]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset, 0.0f),
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

        texture = fontTexture;
    }

    int16_t TTFont::getKerningPair(uint32_t first, uint32_t second)
    {
        auto i = kern.find(std::make_pair(first, second));

        if (i != kern.end())
        {
            return i->second;
        }

        return 0;
    }

    float TTFont::getStringWidth(const std::string& text)
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

    bool TTFont::parseFont(const std::string & filename, uint16_t pt, bool mipmaps, UTFChars flag)
    {
        stbtt_fontinfo font;
        std::vector<unsigned char> data;

        std::string f = ouzel::sharedEngine->getFileSystem()->getPath(filename);

        ouzel::sharedEngine->getFileSystem()->readFile(f, data);
        {
            return false;
        }

        if (!stbtt_InitFont(&font, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0)))
        {
            Log(Log::Level::ERR) << "Failed to load font";
            return false;
        }

        float s = stbtt_ScaleForPixelHeight(&font, pt);

        int w, h, xoff, yoff;
        height = 0;
        width = 0;

        std::vector<uint16_t> glyphs;
        std::map<uint32_t, std::pair<Size2, std::vector<uint8_t>>> glyphToBitmapData;

        if (flag && UTFChars::ASCII)
        {
            for (uint16_t i = 32; i < 127; i++)
            {
                glyphs.push_back(i);
            }
        }
        if (flag && UTFChars::ASCIIPLUS)
        {
            for (uint16_t i = 128; i < 256; i++)
            {
                glyphs.push_back(i);
            }
        }

        for (uint16_t c : glyphs)
        {
            unsigned char* bitmap = stbtt_GetCodepointBitmap(&font, s, s, c, &w, &h, &xoff, &yoff);

            if (bitmap)
            {
                for (uint16_t j : glyphs)
                {
                    int kx = stbtt_GetCodepointKernAdvance(&font, j, c);
                    if (kx == 0) continue;
                    kern.emplace(std::pair<uint32_t, uint32_t>(j, c), static_cast<int16_t>(kx * s));
                }

                int advance, leftBearing;
                stbtt_GetCodepointHMetrics(&font, c, &advance, &leftBearing);
                CharDescriptor nd;
                nd.xAdvance = static_cast<int16_t>(advance * s);
                nd.height = static_cast<int16_t>(h);
                nd.width = static_cast<int16_t>(w);
                nd.xOffset = static_cast<int16_t>(leftBearing * s);
                nd.yOffset = static_cast<int16_t>(h - abs(yoff));

                std::vector<uint8_t> currentBuffer(static_cast<size_t>(h * w));
                std::copy(&bitmap[0], &bitmap[h * w], currentBuffer.begin());

                glyphToBitmapData.emplace(c, std::make_pair(Size2(static_cast<float>(w), static_cast<float>(h)), currentBuffer));
                chars.emplace(c, nd);
                height = height < static_cast<uint16_t>(h) ? static_cast<uint16_t>(h) : height;
                width += static_cast<uint16_t>(w);
            }
        }

        std::vector<std::vector<uint8_t>> scanlines(height);
        int x = 0;
        for (const auto &c : glyphToBitmapData)
        {
            uint16_t charHeight = static_cast<uint16_t>(c.second.first.height());
            uint16_t charWidth = static_cast<uint16_t>(c.second.first.width());
            chars.at(c.first).x = static_cast<int16_t>(x);
            chars.at(c.first).y = 0;
            x += charWidth;

            uint16_t extraRows = height - charHeight;
            chars.at(c.first).yOffset += extraRows;
            size_t extraSpaceSize = extraRows * charWidth;
            unsigned int charSize = charHeight * charWidth;
            std::vector<uint8_t> newCharBuffer(extraSpaceSize + charSize, 0x00);
            std::copy(c.second.second.begin(), c.second.second.begin() + charSize, newCharBuffer.data());
            assert(newCharBuffer.size() == height * charWidth);
            for (uint16_t i = 0; i < height; i++)
            {
                size_t scanlinesPreSize = scanlines[i].size();
                scanlines[i].resize(scanlinesPreSize + charWidth);
                uint8_t* bufferStart = newCharBuffer.data() + static_cast<int>(charWidth * i);
                std::copy(bufferStart, bufferStart + charWidth, scanlines[i].data() + scanlinesPreSize);
            }
        }

        std::vector<uint8_t> textureData(scanlines[0].size() * height * 4);
        for (uint16_t i = 0; i < height; i++)
        {
            for (uint32_t j = 0; j < scanlines[0].size(); j++)
            {
                uint8_t b = scanlines[i][j];
                textureData[(i * scanlines[0].size() + j) * 4 + 0] = 255;
                textureData[(i * scanlines[0].size() + j) * 4 + 1] = 255;
                textureData[(i * scanlines[0].size() + j) * 4 + 2] = 255;
                textureData[(i * scanlines[0].size() + j) * 4 + 3] = b;
            }
        }

        fontTexture = std::make_shared<graphics::Texture>();
        fontTexture->init(textureData, Size2(width, height), 0, mipmaps ? 0 : 1);
        pages = 1;
        lineHeight = pt;
        kernCount = static_cast<uint16_t>(kern.size());
        base = 0;

        return true;
    }
}
