// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "utils/Types.h"
#include "math/AABB2.h"
#include "math/Rectangle.h"
#include "graphics/Vertex.h"

namespace ouzel
{
    namespace scene
    {
        class SpriteFrame
        {
        public:
            static std::vector<SpriteFrame> loadSpriteFrames(const std::string& filename, bool mipmaps = true);

            SpriteFrame(const graphics::TexturePtr& pTexture,
                        const Rectangle& frameRectangle,
                        bool rotated,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            SpriteFrame(const graphics::TexturePtr& pTexture,
                        const std::vector<uint16_t>& indices,
                        const std::vector<graphics::VertexPCT>& vertices,
                        const Rectangle& frameRectangle,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            const Rectangle& getRectangle() const { return rectangle; }

            const AABB2& getBoundingBox() const { return boundingBox; }
            const graphics::MeshBufferPtr& getMeshBuffer() const { return meshBuffer; }
            const graphics::TexturePtr& getTexture() const { return texture; }

        protected:
            Rectangle rectangle;
            AABB2 boundingBox;
            graphics::MeshBufferPtr meshBuffer;
            graphics::IndexBufferPtr indexBuffer;
            graphics::VertexBufferPtr vertexBuffer;
            graphics::TexturePtr texture;
        };
    } // scene
} // ouzel
