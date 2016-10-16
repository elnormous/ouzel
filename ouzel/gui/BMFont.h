// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include <map>
#include "utils/Types.h"
#include "graphics/Color.h"
#include "graphics/Texture.h"
#include "graphics/MeshBuffer.h"

namespace ouzel
{
    class KerningInfo
    {
    public:
        int16_t first = 0;
        int16_t second = 0;
        int16_t amount = 0;
    };

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

    class BMFont
    {
    public:
        BMFont();
        BMFont(const std::string& filename);

        float getHeight() { return lineHeight; }

        void getVertices(const std::string& text, const graphics::Color& color, const Vector2& anchor, std::vector<uint16_t>& indices, std::vector<graphics::VertexPCT>& vertices);

        const graphics::TexturePtr& getTexture() const { return texture; }

    protected:
        bool parseFont(const std::string& filename);
        int32_t getKerningPair(int32_t, int32_t);
        float getStringWidth(const std::string& text);

        int16_t lineHeight = 0;
        int16_t base = 0;
        int16_t width = 0;
        int16_t height = 0;
        int16_t pages = 0;
        int16_t outline = 0;
        uint16_t kernCount = 0;
        std::map<uint32_t, CharDescriptor> chars;
        std::map<std::pair<uint32_t, uint32_t>, KerningInfo> kern;
        graphics::TexturePtr texture;
    };
}
