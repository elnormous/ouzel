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
                                    uint32_t newSampleCount,
                                    TextureFilter newTextureFilter,
                                    PixelFormat newBackBufferFormat,
                                    bool newVerticalSync)
        {
            free();

            return RendererOGL::init(newWindow, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync);
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
