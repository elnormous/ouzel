// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cassert>
#include <stdexcept>
#define STB_TRUETYPE_IMPLEMENTATION
#include "TTFont.hpp"
#include "core/Engine.hpp"
#include "utils/Utf8.hpp"

namespace ouzel
{
    namespace gui
    {
        TTFont::TTFont(const std::vector<uint8_t>& initData, bool initMipmaps):
            data(initData),
            mipmaps(initMipmaps)
        {
            int offset = stbtt_GetFontOffsetForIndex(data.data(), 0);

            if (offset == -1)
                throw std::runtime_error("Not a font");

            font.reset(new stbtt_fontinfo());

            if (!stbtt_InitFont(font.get(), data.data(), offset))
                throw std::runtime_error("Failed to load font");
        }

        void TTFont::getVertices(const std::string& text,
                                 Color color,
                                 float fontSize,
                                 const Vector2F& anchor,
                                 std::vector<uint16_t>& indices,
                                 std::vector<graphics::Vertex>& vertices,
                                 std::shared_ptr<graphics::Texture>& texture)
        {
            if (!font)
                throw std::runtime_error("Font not loaded");

            static constexpr uint32_t SPACING = 2;

            struct CharDescriptor final
            {
                uint16_t x = 0;
                uint16_t y = 0;
                uint16_t width = 0;
                uint16_t height = 0;
                Vector2F offset;
                float advance = 0;
                std::vector<uint8_t> bitmap;
            };

            std::unordered_map<uint32_t, CharDescriptor> chars;

            float s = stbtt_ScaleForPixelHeight(font.get(), fontSize);

            std::vector<uint32_t> utf32Text = utf8::toUtf32(text);

            std::set<uint32_t> glyphs;
            for (uint32_t i : utf32Text)
                glyphs.insert(i);

            uint16_t width = 0;
            uint16_t height = 0;

            int ascent;
            int descent;
            int lineGap;
            stbtt_GetFontVMetrics(font.get(), &ascent, &descent, &lineGap);

            for (uint32_t c : glyphs)
            {
                int w;
                int h;
                int xoff;
                int yoff;

                if (int index = stbtt_FindGlyphIndex(font.get(), static_cast<int>(c)))
                {
                    int advance;
                    int leftBearing;
                    stbtt_GetGlyphHMetrics(font.get(), index, &advance, &leftBearing);

                    CharDescriptor charDesc;

                    if (unsigned char* bitmap = stbtt_GetGlyphBitmapSubpixel(font.get(), s, s, 0.0F, 0.0F, index, &w, &h, &xoff, &yoff))
                    {
                        charDesc.width = static_cast<uint16_t>(w);
                        charDesc.height = static_cast<uint16_t>(h);
                        charDesc.offset.v[0] = static_cast<float>(leftBearing * s);
                        charDesc.offset.v[1] = static_cast<float>(yoff + (ascent - descent) * s);
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

            texture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                          textureData,
                                                          Size2U(width, height), 0,
                                                          mipmaps ? 0 : 1);

            Vector2F position;

            indices.clear();
            vertices.clear();

            indices.reserve(utf32Text.size() * 6);
            vertices.reserve(utf32Text.size() * 4);

            Vector2F textCoords[4];

            size_t firstChar = 0;

            for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
            {
                auto iter = chars.find(*i);

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

                    Vector2F leftTop(f.x / static_cast<float>(width),
                                     f.y / static_cast<float>(height));

                    Vector2F rightBottom((f.x + f.width) / static_cast<float>(width),
                                         (f.y + f.height) / static_cast<float>(height));

                    textCoords[0] = Vector2F(leftTop.v[0], rightBottom.v[1]);
                    textCoords[1] = Vector2F(rightBottom.v[0], rightBottom.v[1]);
                    textCoords[2] = Vector2F(leftTop.v[0], leftTop.v[1]);
                    textCoords[3] = Vector2F(rightBottom.v[0], leftTop.v[1]);

                    vertices.push_back(graphics::Vertex(Vector3F(position.v[0] + f.offset.v[0], -position.v[1] - f.offset.v[1] - f.height, 0.0F),
                                                        color, textCoords[0], Vector3F(0.0F, 0.0F, -1.0F)));
                    vertices.push_back(graphics::Vertex(Vector3F(position.v[0] + f.offset.v[0] + f.width, -position.v[1] - f.offset.v[1] - f.height, 0.0F),
                                                        color, textCoords[1], Vector3F(0.0F, 0.0F, -1.0F)));
                    vertices.push_back(graphics::Vertex(Vector3F(position.v[0] + f.offset.v[0], -position.v[1] - f.offset.v[1], 0.0F),
                                                        color, textCoords[2], Vector3F(0.0F, 0.0F, -1.0F)));
                    vertices.push_back(graphics::Vertex(Vector3F(position.v[0] + f.offset.v[0] + f.width, -position.v[1] - f.offset.v[1], 0.0F),
                                                        color, textCoords[3], Vector3F(0.0F, 0.0F, -1.0F)));

                    if ((i + 1) != utf32Text.end())
                    {
                        int kernAdvance = stbtt_GetCodepointKernAdvance(font.get(),
                                                                        static_cast<int>(*i),
                                                                        static_cast<int>(*(i + 1)));
                        position.v[0] += static_cast<float>(kernAdvance) * s;
                    }

                    position.v[0] += f.advance;
                }

                if (*i == static_cast<uint32_t>('\n') || // line feed
                    (i + 1) == utf32Text.end()) // end of string
                {
                    float lineWidth = position.v[0];
                    position.v[0] = 0.0F;
                    position.v[1] += fontSize + lineGap;

                    for (size_t c = firstChar; c < vertices.size(); ++c)
                        vertices[c].position.v[0] -= lineWidth * anchor.v[0];

                    firstChar = vertices.size();
                }
            }

            float textHeight = position.v[1];

            for (size_t c = 0; c < vertices.size(); ++c)
                vertices[c].position.v[1] += textHeight * (1.0F - anchor.v[1]);
        }
    } // namespace gui
} // namespace ouzel
