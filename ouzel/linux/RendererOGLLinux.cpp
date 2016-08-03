// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLLinux.h"
#include "core/Engine.h"
#include "WindowLinux.h"

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

        bool RendererOGLLinux::init(const WindowPtr& window,
                                    uint32_t newSampleCount,
                                    TextureFiltering newTextureFiltering,
                                    float newTargetFPS,
                                    bool newVerticalSync)
        {
            free();

            return RendererOGL::init(window, newSampleCount, newTextureFiltering, newTargetFPS, newVerticalSync);
        }

        bool RendererOGLLinux::present()
        {
            if (!RendererOGL::present())
            {
                return false;
            }

            std::shared_ptr<WindowLinux> windowLinux = std::static_pointer_cast<WindowLinux>(sharedEngine->getWindow());

            glXSwapBuffers(windowLinux->getDisplay(), windowLinux->getNativeWindow());

            return true;
        }
    } // namespace graphics
} // namespace ouzel
