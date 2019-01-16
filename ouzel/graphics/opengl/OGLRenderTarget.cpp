// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGLRenderTarget.hpp"
#include "OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        OGLRenderTarget::OGLRenderTarget(OGLRenderDevice& renderDeviceOGL):
            OGLRenderResource(renderDeviceOGL)
        {
            glGenFramebuffersProc(1, &frameBufferId);
        }

        OGLRenderTarget::~OGLRenderTarget()
        {
            if (frameBufferId)
                renderDevice.deleteFrameBuffer(frameBufferId);
        }

        void OGLRenderTarget::reload()
        {
            glGenFramebuffersProc(1, &frameBufferId);
        }
    } // namespace graphics
} // namespace ouzel

#endif
