// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <algorithm>
#include <memory>
#include "Types.h"
#include "Noncopyable.h"
#include "Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget: public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~RenderTarget();

            virtual bool init(const Size2& newSize, bool useDepthBuffer);

            TexturePtr getTexture() const { return texture; }

        protected:
            RenderTarget();

            Size2 size;
            bool depthBuffer = false;

            TexturePtr texture;
        };
    } // namespace graphics
} // namespace ouzel
