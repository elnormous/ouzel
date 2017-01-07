// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLLinux.h"
#include "core/linux/WindowLinux.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLLinux::~RendererOGLLinux()
        {
        }

        void RendererOGLLinux::free()
        {
        }

        bool RendererOGLLinux::init(Window* newWindow,
                                    const Size2& newSize,
                                    uint32_t newSampleCount,
                                    TextureFilter newTextureFilter,
                                    PixelFormat newBackBufferFormat,
                                    bool newVerticalSync,
                                    uint32_t newDepthBits)
        {
            free();

            return RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepthBits);
        }

        bool RendererOGLLinux::present()
        {
            if (!RendererOGL::present())
            {
                return false;
            }

            WindowLinux* windowLinux = static_cast<WindowLinux*>(window);

            glXSwapBuffers(windowLinux->getDisplay(), windowLinux->getNativeWindow());

            return true;
        }
    } // namespace graphics
} // namespace ouzel
