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
    namespace video
    {
        class Renderer;
        class Texture;

        class RenderTarget: public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~RenderTarget();

            virtual bool init(const Size2& size, bool depthBuffer);

            TexturePtr getTexture() const { return _texture; }

        protected:
            RenderTarget();

            Size2 _size;
            bool _depthBuffer = false;

            TexturePtr _texture;
        };
    } // namespace video
} // namespace ouzel
