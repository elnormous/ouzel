// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include <map>
#include "Types.h"
#include "Color.h"
#include "Texture.h"
#include "MeshBuffer.h"

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
        ~BMFont();

        bool loadFont(const std::string& filename);
        float getHeight() { return _lineHeight; }

        void getVertices(const std::string& text, const graphics::Color& color, const Vector2& anchor, std::vector<uint16_t>& indices, std::vector<graphics::VertexPCT>& vertices);

        const graphics::TexturePtr& getTexture() const { return _texture; }

    private:
        bool parseFont(const std::string& filename);
        int32_t getKerningPair(int32_t, int32_t);
        float getStringWidth(const std::string& text);

        int16_t _lineHeight = 0;
        int16_t _base = 0;
        int16_t _width = 0;
        int16_t _height = 0;
        int16_t _pages = 0;
        int16_t _outline = 0;
        uint16_t _kernCount = 0;
        std::map<int32_t, CharDescriptor> _chars;
        std::map<std::pair<int32_t, int32_t>, KerningInfo> _kern;
        graphics::TexturePtr _texture;
    };
}
