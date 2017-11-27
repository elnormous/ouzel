// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "math/Box2.hpp"
#include "math/Rectangle.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"

namespace ouzel
{
    namespace scene
    {
        class SpriteFrame
        {
        public:
            SpriteFrame(const std::string& frameName,
                        const Size2& textureSize,
                        const Rectangle& frameRectangle,
                        bool rotated,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            SpriteFrame(const std::string& frameName,
                        const std::vector<uint16_t>& indices,
                        const std::vector<graphics::Vertex>& vertices,
                        const Rectangle& frameRectangle,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            const std::string& getName() const { return name; }
            const Rectangle& getRectangle() const { return rectangle; }

            const Box2& getBoundingBox() const { return boundingBox; }
            const std::shared_ptr<graphics::MeshBuffer>& getMeshBuffer() const { return meshBuffer; }

        protected:
            std::string name;
            Rectangle rectangle;
            Box2 boundingBox;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // scene
} // ouzel
