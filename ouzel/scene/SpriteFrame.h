// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "utils/Noncopyable.h"
#include "utils/Types.h"
#include "math/Rectangle.h"
#include "graphics/Vertex.h"

namespace ouzel
{
    namespace scene
    {
        class SpriteFrame: public Noncopyable
        {
        public:
            static std::vector<SpriteFramePtr> loadSpriteFrames(const std::string& filename, bool mipmaps = true);
            static SpriteFramePtr create(const Rectangle& rectangle, const graphics::TexturePtr& texture, bool rotated, const Size2& sourceSize, const Vector2& sourceOffset, const Vector2& pivot);

            SpriteFrame(Rectangle pRectangle,
                        graphics::MeshBufferPtr pMeshBuffer,
                        graphics::TexturePtr pTexture):
                rectangle(pRectangle),
                meshBuffer(pMeshBuffer),
                texture(pTexture)
            {
            }

            const Rectangle& getRectangle() const { return rectangle; }
            const graphics::MeshBufferPtr& getMeshBuffer() const { return meshBuffer; }
            const graphics::TexturePtr& getRexture() const { return texture; }

        protected:
            Rectangle rectangle;
            graphics::MeshBufferPtr meshBuffer;
            graphics::TexturePtr texture;
        };
    } // scene
} // ouzel
