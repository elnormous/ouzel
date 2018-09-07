// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "stb_truetype.h"
#include "gui/Font.hpp"

namespace ouzel
{
    class TTFont: public Font
    {
    public:
        TTFont();
        TTFont(const std::vector<uint8_t>& newData, bool newMipmaps = true);

        virtual void getVertices(const std::string& text,
                                 Color color,
                                 float fontSize,
                                 const Vector2& anchor,
                                 std::vector<uint16_t>& indices,
                                 std::vector<graphics::Vertex>& vertices,
                                 std::shared_ptr<graphics::Texture>& texture) override;

    private:
        int16_t getKerningPair(uint32_t, uint32_t);
        float getStringWidth(const std::string& text);

        bool loaded = false;
        bool mipmaps = true;
        stbtt_fontinfo font;
        std::vector<unsigned char> data;
    };
}
