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
        RenderTarget::RenderTarget()
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

            update();

            return true;
        }

        void RenderTarget::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            update();
        }

        void RenderTarget::setClearDepthBuffer(bool clear)
        {
            clearColorBuffer = clear;

            update();
        }

        void RenderTarget::setClearColor(Color color)
        {
            clearColor = color;

            update();
        }

        void RenderTarget::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            currentData.size = size;
            currentData.clearColor = clearColor;
            currentData.depthBuffer = depthBuffer;
            currentData.clearColorBuffer = clearColorBuffer;
            currentData.clearDepthBuffer = clearDepthBuffer;

            dirty = true;
        }

        bool RenderTarget::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dirty = false;
            uploadData = std::move(currentData);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
