// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "stb_truetype.h"
#include "gui/Font.hpp"

namespace ouzel
{
    class TTFont : public Font
    {
    public:
        TTFont();
        TTFont(const std::string& filename, bool aMipmaps = true);

        bool init(const std::string& filename);

        virtual void getVertices(const std::string& text,
                                 const Color& color,
                                 float fontSize,
                                 const Vector2& anchor,
                                 std::vector<uint16_t>& indices,
                                 std::vector<graphics::VertexPCT>& vertices,
                                 std::shared_ptr<graphics::Texture>& texture) override;

    protected:
        int16_t getKerningPair(uint32_t, uint32_t);
        float getStringWidth(const std::string& text);

        bool mipmaps = true;
        stbtt_fontinfo font;
        std::vector<unsigned char> data;
    };
}
