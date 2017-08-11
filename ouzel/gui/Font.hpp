// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "math/Color.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    class Font: public Noncopyable
    {
    public:
        uint16_t getHeight() { return lineHeight; }

        void getVertices(const std::string& text,
                         const Color& color,
                         const Vector2& anchor,
                         const Vector2& scale,
                         std::vector<uint16_t>& indices,
                         std::vector<graphics::VertexPCT>& vertices);

        const std::shared_ptr<graphics::Texture>& getTexture() const { return texture; }

    protected:
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
        std::shared_ptr<graphics::Texture> texture;
    };
}
