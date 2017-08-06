// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "math/Box2.hpp"
#include "math/Rectangle.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        class SpriteFrame
        {
        public:
            static std::vector<SpriteFrame> loadSpriteFrames(const std::string& filename, bool mipmaps = true);

            SpriteFrame(const std::shared_ptr<graphics::Texture>& pTexture,
                        const Rectangle& frameRectangle,
                        bool rotated,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            SpriteFrame(const std::shared_ptr<graphics::Texture>& pTexture,
                        const std::vector<uint16_t>& indices,
                        const std::vector<graphics::VertexPCT>& vertices,
                        const Rectangle& frameRectangle,
                        const Size2& sourceSize,
                        const Vector2& sourceOffset,
                        const Vector2& pivot);

            const Rectangle& getRectangle() const { return rectangle; }

            const Box2& getBoundingBox() const { return boundingBox; }
            const std::shared_ptr<graphics::MeshBuffer>& getMeshBuffer() const { return meshBuffer; }
            const std::shared_ptr<graphics::Texture>& getTexture() const { return texture; }

        protected:
            Rectangle rectangle;
            Box2 boundingBox;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
            std::shared_ptr<graphics::Texture> texture;
        };
    } // scene
} // ouzel
