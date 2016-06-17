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

            SpriteFrame(Rectangle pRectangle,
                        graphics::MeshBufferPtr pMeshBuffer,
                        graphics::TexturePtr pTexture);
            SpriteFrame(const Rectangle& pRectangle,
                        const graphics::TexturePtr& pTexture,
                        bool rotated, const Size2& sourceSize,
                        const Vector2& sourceOffset, const Vector2& pivot);

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
