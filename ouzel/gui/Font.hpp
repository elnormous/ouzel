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
        virtual ~Font();

        virtual void getVertices(const std::string& text,
                                 const Color& color,
                                 const Vector2& anchor,
                                 const Vector2& scale,
                                 std::vector<uint16_t>& indices,
                                 std::vector<graphics::VertexPCT>& vertices,
                                 std::shared_ptr<graphics::Texture>& texture) = 0;
    };
}
