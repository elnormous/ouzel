// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "Font.hpp"

namespace ouzel
{
    class BMFont final: public Font
    {
    public:
        BMFont();
        explicit BMFont(const std::vector<uint8_t>& data);

        void getVertices(const std::string& text,
                         Color color,
                         float fontSize,
                         const Vector2& anchor,
                         std::vector<uint16_t>& indices,
                         std::vector<graphics::Vertex>& vertices,
                         std::shared_ptr<graphics::Texture>& texture) override;

        float getStringWidth(const std::string& text);

    private:
        int16_t getKerningPair(uint32_t, uint32_t);

        class CharDescriptor
        {
        public:
            int16_t x = 0;
            int16_t y = 0;
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
