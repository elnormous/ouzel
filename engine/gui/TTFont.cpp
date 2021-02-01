// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include "TTFont.hpp"
#include "../core/Engine.hpp"
#include "../utils/Utf8.hpp"

#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  if defined(__clang__)
#    pragma GCC diagnostic ignored "-Wcomma"
#  endif
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

namespace ouzel::gui
{
    TTFont::TTFont(const std::vector<std::byte>& initData, bool initMipmaps):
        data(initData),
        mipmaps(initMipmaps)
    {
        const auto offset = stbtt_GetFontOffsetForIndex(reinterpret_cast<const unsigned char*>(data.data()), 0);

        if (offset == -1)
            throw std::runtime_error("Not a font");

        font = std::make_unique<stbtt_fontinfo>();

        if (!stbtt_InitFont(font.get(), reinterpret_cast<const unsigned char*>(data.data()), offset))
            throw std::runtime_error("Failed to load font");
    }

    Font::RenderData TTFont::getRenderData(const std::string& text,
                                           Color color,
                                           float fontSize,
                                           const Vector2F& anchor) const
    {
        if (!font)
            throw std::runtime_error("Font not loaded");

        constexpr std::uint32_t spacing = 2U;

        const float s = stbtt_ScaleForPixelHeight(font.get(), fontSize);

        const std::u32string utf32Text = utf8::toUtf32(text);

        std::set<char32_t> glyphs;
        for (const char32_t i : utf32Text)
            glyphs.insert(i);

        std::uint16_t width = 0;
        std::uint16_t height = 0;

        int ascent;
        int descent;
        int lineGap;
        stbtt_GetFontVMetrics(font.get(), &ascent, &descent, &lineGap);

        struct CharDescriptor final
        {
            std::uint16_t x = 0;
            std::uint16_t y = 0;
            std::uint16_t width = 0;
            std::uint16_t height = 0;
            Vector2F offset;
            float advance = 0;
            std::vector<std::uint8_t> bitmap;
        };

        std::unordered_map<std::uint32_t, CharDescriptor> chars;

        for (const char32_t c : glyphs)
        {
            int w;
            int h;
            int xoff;
            int yoff;

            if (const auto index = stbtt_FindGlyphIndex(font.get(), static_cast<int>(c)))
            {
                int advance;
                int leftBearing;
                stbtt_GetGlyphHMetrics(font.get(), index, &advance, &leftBearing);

                CharDescriptor charDesc;

                if (const auto bitmap = stbtt_GetGlyphBitmapSubpixel(font.get(), s, s, 0.0F, 0.0F, index, &w, &h, &xoff, &yoff))
                {
                    charDesc.width = static_cast<std::uint16_t>(w);
                    charDesc.height = static_cast<std::uint16_t>(h);
                    charDesc.offset.v[0] = static_cast<float>(leftBearing * s);
                    charDesc.offset.v[1] = static_cast<float>(yoff + (ascent - descent) * s);
                    charDesc.bitmap = std::vector<std::uint8_t>(bitmap, bitmap + h * w);
                    charDesc.x = width;

                    width += static_cast<std::uint16_t>(w);
                    height = std::max(height, static_cast<std::uint16_t>(h));

                    stbtt_FreeBitmap(bitmap, nullptr);
                }

                charDesc.advance = static_cast<float>(advance * s);

                if (!chars.empty())
                    width += spacing;

                chars[c] = charDesc;
            }
        }

        std::vector<std::uint8_t> textureData(width * height * 4);

        for (std::uint16_t posX = 0; posX < width; ++posX)
        {
            for (std::uint16_t posY = 0; posY < height; ++posY)
            {
                textureData[(posY * width + posX) * 4 + 0] = 255;
                textureData[(posY * width + posX) * 4 + 1] = 255;
                textureData[(posY * width + posX) * 4 + 2] = 255;
                textureData[(posY * width + posX) * 4 + 3] = 0;
            }
        }

        for (const auto& c : chars)
        {
            const auto& charDesc = c.second;

            for (std::uint16_t posX = 0; posX < charDesc.width; ++posX)
            {
                for (std::uint16_t posY = 0; posY < charDesc.height; ++posY)
                {
                    textureData[(posY * width + posX + charDesc.x) * 4 + 0] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 1] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 2] = 255;
                    textureData[(posY * width + posX + charDesc.x) * 4 + 3] = charDesc.bitmap[posY * charDesc.width + posX];
                }
            }
        }

        RenderData result;

        auto texture = std::make_shared<graphics::Texture>(*engine->getGraphics(),
                                                           textureData,
                                                           Size2U(width, height),
                                                           graphics::Flags::none,
                                                           mipmaps ? 0 : 1);

        Vector2F position;

        std::vector<std::uint16_t> indices;
        std::vector<graphics::Vertex> vertices;
        indices.reserve(utf32Text.size() * 6);
        vertices.reserve(utf32Text.size() * 4);

        Vector2F textCoords[4];

        std::size_t firstChar = 0;

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            auto iter = chars.find(*i);

            if (iter != chars.end())
            {
                const auto& f = iter->second;

                const auto startIndex = static_cast<std::uint16_t>(vertices.size());
                indices.push_back(startIndex + 0);
                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 2);

                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 3);
                indices.push_back(startIndex + 2);

                Vector2F leftTop(f.x / static_cast<float>(width),
                                 f.y / static_cast<float>(height));

                Vector2F rightBottom((f.x + f.width) / static_cast<float>(width),
                                     (f.y + f.height) / static_cast<float>(height));

                textCoords[0] = Vector2F(leftTop.v[0], rightBottom.v[1]);
                textCoords[1] = Vector2F(rightBottom.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2F(leftTop.v[0], leftTop.v[1]);
                textCoords[3] = Vector2F(rightBottom.v[0], leftTop.v[1]);

                vertices.emplace_back(Vector3F{position.v[0] + f.offset.v[0], -position.v[1] - f.offset.v[1] - f.height, 0.0F},
                                      color, textCoords[0], Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{position.v[0] + f.offset.v[0] + f.width, -position.v[1] - f.offset.v[1] - f.height, 0.0F},
                                      color, textCoords[1], Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{position.v[0] + f.offset.v[0], -position.v[1] - f.offset.v[1], 0.0F},
                                      color, textCoords[2], Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{position.v[0] + f.offset.v[0] + f.width, -position.v[1] - f.offset.v[1], 0.0F},
                                      color, textCoords[3], Vector3F{0.0F, 0.0F, -1.0F});

                if ((i + 1) != utf32Text.end())
                {
                    const auto kernAdvance = stbtt_GetCodepointKernAdvance(font.get(),
                                                                          static_cast<int>(*i),
                                                                          static_cast<int>(*(i + 1)));
                    position.v[0] += static_cast<float>(kernAdvance) * s;
                }

                position.v[0] += f.advance;
            }

            if (*i == static_cast<std::uint32_t>('\n') || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                const float lineWidth = position.v[0];
                position.v[0] = 0.0F;
                position.v[1] += fontSize + lineGap;

                for (std::size_t c = firstChar; c < vertices.size(); ++c)
                    vertices[c].position.v[0] -= lineWidth * anchor.v[0];

                firstChar = vertices.size();
            }
        }

        const float textHeight = position.v[1];

        for (graphics::Vertex& vertex : vertices)
            vertex.position.v[1] += textHeight * (1.0F - anchor.v[1]);

        return std::make_tuple(std::move(indices), std::move(vertices), std::move(texture));
    }
}
