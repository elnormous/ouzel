// Copyright (C) 2018 Elviss Strazdins
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
    TTFont::TTFont()
    {
    }

    TTFont::TTFont(const std::string& filename, bool initMipmaps)
    {
        init(filename, initMipmaps);
    }

    bool TTFont::init(const std::string & filename, bool newMipmaps)
    {
        loaded = false;
        mipmaps = newMipmaps;

        engine->getFileSystem()->readFile(engine->getFileSystem()->getPath(filename), data);

        if (!stbtt_InitFont(&font, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0)))
        {
            Log(Log::Level::ERR) << "Failed to load font";
            return false;
        }

        loaded = true;

        return true;
    }

    bool TTFont::init(const std::vector<uint8_t>& newData, bool newMipmaps)
    {
        loaded = false;
        data = newData;
        mipmaps = newMipmaps;

        if (!stbtt_InitFont(&font, data.data(), stbtt_GetFontOffsetForIndex(data.data(), 0)))
        {
            Log(Log::Level::ERR) << "Failed to load font";
            return false;
        }

        loaded = true;

        return true;
    }

    bool TTFont::getVertices(const std::string& text,
                             const Color& color,
                             float fontSize,
                             const Vector2& anchor,
                             std::vector<uint16_t>& indices,
                             std::vector<graphics::Vertex>& vertices,
                             std::shared_ptr<graphics::Texture>& texture)
    {
        if (!loaded) return false;

        static const uint32_t SPACING = 2;

        struct CharDescriptor
        {
            uint16_t x = 0;
            uint16_t y = 0;
            uint16_t width = 0;
            uint16_t height = 0;
            Vector2 offset;
            float advance = 0;
            std::vector<uint8_t> bitmap;
        };

        std::unordered_map<uint32_t, CharDescriptor> chars;

        float s = stbtt_ScaleForPixelHeight(&font, fontSize);

        std::vector<uint32_t> utf32Text = utf8ToUtf32(text);

        std::set<uint32_t> glyphs;
        for (uint32_t i : utf32Text)
            glyphs.insert(i);

        uint16_t width = 0;
        uint16_t height = 0;

        int ascent;
        int descent;
        int lineGap;
        stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

        for (uint32_t c : glyphs)
        {
            int w;
            int h;
            int xoff;
            int yoff;

            if (int index = stbtt_FindGlyphIndex(&font, static_cast<int>(c)))
            {
                int advance;
                int leftBearing;
                stbtt_GetGlyphHMetrics(&font, index, &advance, &leftBearing);

                CharDescriptor charDesc;

                if (unsigned char* bitmap = stbtt_GetGlyphBitmapSubpixel(&font, s, s, 0.0F, 0.0F, index, &w, &h, &xoff, &yoff))
                {
                    charDesc.width = static_cast<uint16_t>(w);
                    charDesc.height = static_cast<uint16_t>(h);
                    charDesc.offset.x = static_cast<float>(leftBearing * s);
                    charDesc.offset.y = static_cast<float>(yoff + (ascent - descent) * s);
                    charDesc.bitmap = std::vector<uint8_t>(bitmap, bitmap + h * w);
                    charDesc.x = width;

                    width += static_cast<uint16_t>(w);
                    height = height > static_cast<uint16_t>(h) ? height : static_cast<uint16_t>(h);

                    stbtt_FreeBitmap(bitmap, nullptr);
                }

                charDesc.advance = static_cast<float>(advance * s);

                if (!chars.empty())
                    width += SPACING;

                chars[c] = charDesc;
            }
        }

        std::vector<uint8_t> textureData(width * height * 4);

        for (uint16_t posX = 0; posX < width; ++posX)
        {
            for (uint16_t posY = 0; posY < height; ++posY)
            {
                textureData[(posY * width + posX) * 4 + 0] = 255;
                textureData[(posY * width + posX) * 4 + 1] = 255;
                textureData[(posY * width + posX) * 4 + 2] = 255;
                textureData[(posY * width + posX) * 4 + 3] = 0;
            }
        }

        for (auto& c : chars)
        {
            CharDescriptor& charDesc = c.second;

            for (uint16_t posX = 0; posX < charDesc.width; ++posX)
            {
                for (uint16_t posY = 0; posY < charDesc.height; ++posY)
                {
                    textureData[(posY * width + posX + charDesc.x) * 4 + 0] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 1] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 2] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 3] = charDesc.bitmap[posY * charDesc.width + posX];
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

                vertices.push_back(graphics::Vertex(Vector3(position.x + f.offset.x, -position.y - f.offset.y - f.height, 0.0F),
                                                    color, textCoords[0], Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(position.x + f.offset.x + f.width, -position.y - f.offset.y - f.height, 0.0F),
                                                    color, textCoords[1], Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(position.x + f.offset.x, -position.y - f.offset.y, 0.0F),
                                                    color, textCoords[2], Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(position.x + f.offset.x + f.width, -position.y - f.offset.y, 0.0F),
                                                    color, textCoords[3], Vector3(0.0F, 0.0F, -1.0F)));

                if ((i + 1) != utf32Text.end())
                {
                    int kernAdvance = stbtt_GetCodepointKernAdvance(&font,
                                                                    static_cast<int>(*i),
                                                                    static_cast<int>(*(i + 1)));
                    position.x += static_cast<float>(kernAdvance) * s;
                }

                position.x += f.advance;
            }

            if (*i == static_cast<uint32_t>('\n') || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                float lineWidth = position.x;
                position.x = 0.0F;
                position.y += fontSize + lineGap;

                for (size_t c = firstChar; c < vertices.size(); ++c)
                    vertices[c].position.x -= lineWidth * anchor.x;

                firstChar = vertices.size();
            }
        }

        float textHeight = position.y;

        for (size_t c = 0; c < vertices.size(); ++c)
            vertices[c].position.y += textHeight * (1.0F - anchor.y);

        return true;
    }
}
