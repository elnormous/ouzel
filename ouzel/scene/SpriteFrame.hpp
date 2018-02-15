// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "math/Box2.hpp"
#include "math/Rect.hpp"
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
                        const Rect& frameRectangle,
                        bool rotated,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            SpriteFrame(const std::string& frameName,
                        const std::vector<uint16_t>& indices,
                        const std::vector<graphics::Vertex>& vertices,
                        const Rect& frameRectangle,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            inline const std::string& getName() const { return name; }
            inline const Rect& getRectangle() const { return rectangle; }

            inline const Box2& getBoundingBox() const { return boundingBox; }
            inline const std::shared_ptr<graphics::MeshBuffer>& getMeshBuffer() const { return meshBuffer; }

        protected:
            std::string name;
            Rect rectangle;
            Box2 boundingBox;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // scene
} // ouzel
