// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>

#define STB_TRUETYPE_IMPLEMENTATION
#include "TTFont.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    TTFont::TTFont():
        Font()
    {
    }

    TTFont::TTFont(const std::string& filename, bool aMipmaps):
        mipmaps(aMipmaps)
    {
        if (!parseFont(filename))
        {
            Log(Log::Level::ERR) << "Failed to parse font " << filename;
        }
    }

    void TTFont::getVertices(const std::string& text,
                             const Color& color,
                             float fontSize,
                             const Vector2& anchor,
                             std::vector<uint16_t>& indices,
                             std::vector<graphics::VertexPCT>& vertices,
                             std::shared_ptr<graphics::Texture>& texture)
    {
        struct CharDescriptor
        {
            uint16_t x = 0, y = 0;
            uint16_t width = 0;
            uint16_t height = 0;
            int16_t xOffset = 0;
            int16_t yOffset = 0;
            int16_t xAdvance = 0;
        };

        uint16_t width = 0;
        uint16_t height = 0;
        std::unordered_map<uint32_t, CharDescriptor> chars;

        float s = stbtt_ScaleForPixelHeight(&font, fontSize);

        std::set<uint32_t> glyphs;
        std::map<uint32_t, std::pair<Size2, std::vector<uint8_t>>> glyphToBitmapData;

        std::vector<uint32_t> utf32Text = utf8to32(text);

        for (uint32_t i = 0; i < 127; ++i)
        //for (uint32_t i : utf32Text)
        {
            glyphs.insert(i);
        }

        for (uint32_t c : glyphs)
        {
            int w, h, xoff, yoff;
            unsigned char* bitmap = stbtt_GetCodepointBitmap(&font, s, s, static_cast<int>(c), &w, &h, &xoff, &yoff);

            if (bitmap)
            {
                int advance, leftBearing;
                stbtt_GetCodepointHMetrics(&font, static_cast<int>(c), &advance, &leftBearing);
                CharDescriptor charDesc;
                charDesc.xAdvance = static_cast<int16_t>(advance * s);
                charDesc.height = static_cast<uint16_t>(h);
                charDesc.width = static_cast<uint16_t>(w);
                charDesc.xOffset = static_cast<int16_t>(leftBearing * s);
                charDesc.yOffset = static_cast<int16_t>(yoff);

                glyphToBitmapData[c] = std::make_pair(Size2(static_cast<float>(w), static_cast<float>(h)), std::vector<uint8_t>(bitmap, bitmap + h * w));
                chars[c] = charDesc;

                width += static_cast<uint16_t>(w);
                height = height > static_cast<uint16_t>(h) ? height : static_cast<uint16_t>(h);
            }
        }

        std::vector<uint8_t> textureData(width * height * 4);
        uint16_t x = 0;
        for (const auto& c : glyphToBitmapData)
        {
            uint16_t charWidth = static_cast<uint16_t>(c.second.first.width);
            uint16_t charHeight = static_cast<uint16_t>(c.second.first.height);

            CharDescriptor& charDesc = chars[c.first];
            charDesc.x = x;
            x += charWidth;

            for (uint16_t posX = 0; posX < charWidth; ++posX)
            {
                for (uint16_t posY = 0; posY < charHeight; ++posY)
                {
                    textureData[(posY * width + posX + charDesc.x) * 4 + 0] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 1] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 2] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 3] = c.second.second[posY * charWidth + posX];
                }

                // add empty lines
                for (uint16_t posY = charHeight; posY < height; ++posY)
                {
                    textureData[(posY * width + posX + charDesc.x) * 4 + 0] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 1] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 2] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 3] = 0;
                }
            }
        }

        texture = std::make_shared<graphics::Texture>();
        texture->init(textureData, Size2(width, height), 0, mipmaps ? 0 : 1);

        Vector2 position;

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

                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset, -position.y - f.yOffset - f.height, 0.0f),
                                                       color, textCoords[0]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset + f.width, -position.y - f.yOffset - f.height, 0.0f),
                                                       color, textCoords[1]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset, -position.y - f.yOffset, 0.0f),
                                                       color, textCoords[2]));
                vertices.push_back(graphics::VertexPCT(Vector3(position.x + f.xOffset + f.width, -position.y - f.yOffset, 0.0f),
                                                       color, textCoords[3]));

                if ((i + 1) != utf32Text.end())
                {
                    int kernAdvance = stbtt_GetCodepointKernAdvance(&font,
                                                                    static_cast<int>(*i),
                                                                    static_cast<int>(*(i + 1)));
                    position.x += static_cast<float>(kernAdvance) * s;
                }

                position.x += f.xAdvance;
            }

            if (*i == static_cast<uint32_t>('\n') || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                float lineWidth = position.x;
                position.x = 0.0f;
                position.y += fontSize;

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
            vertices[c].position.y += textHeight * (1.0f - anchor.y);
        }
    }

    bool TTFont::parseFont(const std::string & filename)
    {
        if (!sharedEngine->getFileSystem()->readFile(sharedEngine->getFileSystem()->getPath(filename), data))
        {
            return false;
        }

        if (!stbtt_InitFont(&font, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0)))
        {
            Log(Log::Level::ERR) << "Failed to load font";
            return false;
        }

        return true;
    }
}
