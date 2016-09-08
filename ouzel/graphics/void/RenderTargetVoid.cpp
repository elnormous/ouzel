// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetVoid.h"
#include "TextureVoid.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetVoid::RenderTargetVoid()
        {
        }

        RenderTargetVoid::~RenderTargetVoid()
        {
        }

        bool RenderTargetVoid::init(const Size2& newSize, bool depthBuffer)
        {
            free();

            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            std::shared_ptr<TextureVoid> textureVoid(new TextureVoid());

            if (!textureVoid->init(size, false, false, true))
            {
                return false;
            }

            texture = textureVoid;
            
            return true;
        }

        bool RenderTargetVoid::upload()
        {
            ready = true;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
