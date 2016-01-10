// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include <map>
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
        
        bool loadFont(std::string const& filename);
        void setColor(Color const& color) { _color = color; }
        void setBlend(int b) { fblend = b; }
        float getHeight() { return lineHeight; }
        
        std::shared_ptr<MeshBuffer> createMeshBuffer(std::string const& text);
        void printCenter(float y, std::string const& text);
        
        std::shared_ptr<Texture> const& getTexture() const { return _texture; }

    private:
        bool parseFont(std::string const& filename);
        int32_t getKerningPair(int32_t, int32_t);
        float getStringWidth(std::string const& text);
        
        int16_t lineHeight = 0;
        int16_t base = 0;
        int16_t width = 0;
        int16_t height = 0;
        int16_t pages = 0;
        int16_t outline = 0;
        uint16_t kernCount = 0;
        std::map<int32_t, CharDescriptor> chars;
        std::vector<KerningInfo> kern;
        Color _color = Color(255,255,255,255);
        std::shared_ptr<Texture> _texture;
        int32_t fblend = 0;
    };
}
