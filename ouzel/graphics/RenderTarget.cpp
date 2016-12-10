// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTarget.h"
#include "Renderer.h"
#include "Texture.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTarget::RenderTarget():
            projectionTransform(Matrix4::IDENTITY)
        {
        }

        RenderTarget::~RenderTarget()
        {
        }

        void RenderTarget::free()
        {
            texture.reset();
        }

        bool RenderTarget::init(const Size2& newSize, bool useDepthBuffer)
        {
            size = newSize;
            depthBuffer = useDepthBuffer;

            texture = sharedEngine->getRenderer()->createTexture();

            if (!texture->init(size, false, false, true))
            {
                return false;
            }

            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void RenderTarget::setClear(bool newClear)
        {
            clear = newClear;
            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
        }

        void RenderTarget::setClearColor(Color color)
        {
            clearColor = color;
            dirty = true;
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
        }

        void RenderTarget::update()
        {
            uploadData.size = size;
            uploadData.clearColor = clearColor;
            uploadData.depthBuffer = depthBuffer;
            uploadData.clear = clear;
            uploadData.dirty = dirty;

            dirty = false;
        }
    } // namespace graphics
} // namespace ouzel
