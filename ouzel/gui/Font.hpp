// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_FONT_HPP
#define OUZEL_GUI_FONT_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "math/Color.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    class Font
    {
    public:
        Font() = default;
        virtual ~Font();

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        Font(Font&&) = delete;
        Font& operator=(Font&&) = delete;

        struct RenderData
        {
            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;
            std::unique_ptr<graphics::Texture> ownedTexture;
            graphics::Texture* texture = nullptr;
        };

        virtual RenderData render(const std::string& text,
                                  Color color,
                                  float fontSize,
                                  const Vector2F& anchor) = 0;
    };
}

#endif // OUZEL_GUI_FONT_HPP
