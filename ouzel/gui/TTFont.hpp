// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "gui/Font.hpp"

namespace ouzel
{
    enum UTFChars
    {
        ASCII = 1,
        ASCIIPLUS = 1 << 1

    };
    class TTFont : public Font
    {
    public:
        TTFont();
        TTFont(const std::string& filename, uint16_t pt, bool mipmaps = true, UTFChars flag = ASCII);

        virtual void getVertices(const std::string& text,
                                 const Color& color,
                                 const Vector2& anchor,
                                 const Vector2& scale,
                                 std::vector<uint16_t>& indices,
                                 std::vector<graphics::VertexPCT>& vertices,
                                 std::shared_ptr<graphics::Texture>& texture) override;

    protected:
        bool parseFont(const std::string& filename, uint16_t pt, bool mipmaps = true, UTFChars flag = ASCII);

        int16_t getKerningPair(uint32_t, uint32_t);
        float getStringWidth(const std::string& text);

        class CharDescriptor
        {
        public:
            int16_t x = 0, y = 0;
            int16_t width = 0;
            int16_t height = 0;
            int16_t xOffset = 0;
            int16_t yOffset = 0;
            int16_t xAdvance = 0;
            int16_t page = 0;
        };

        uint16_t lineHeight = 0;
        uint16_t base = 0;
        uint16_t width = 0;
        uint16_t height = 0;
        uint16_t pages = 0;
        uint16_t outline = 0;
        uint16_t kernCount = 0;
        std::unordered_map<uint32_t, CharDescriptor> chars;
        std::map<std::pair<uint32_t, uint32_t>, int16_t> kern;
        std::shared_ptr<graphics::Texture> fontTexture;
    };
}
