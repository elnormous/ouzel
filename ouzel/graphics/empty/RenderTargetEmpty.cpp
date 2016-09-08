// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetEmpty.h"
#include "TextureEmpty.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetEmpty::RenderTargetEmpty()
        {
        }

        RenderTargetEmpty::~RenderTargetEmpty()
        {
        }

        bool RenderTargetEmpty::init(const Size2& newSize, bool depthBuffer)
        {
            free();

            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            std::shared_ptr<TextureEmpty> textureEmpty(new TextureEmpty());

            if (!textureEmpty->init(size, false, false, true))
            {
                return false;
            }

            texture = textureEmpty;
            
            return true;
        }

        bool RenderTargetEmpty::upload()
        {
            ready = true;
            return true;
        }
    } // namespace graphics
} // namespace ouzel
