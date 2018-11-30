// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "RenderTargetOGL.hpp"
#include "RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetOGL::RenderTargetOGL(RenderDeviceOGL& renderDeviceOGL):
            RenderResourceOGL(renderDeviceOGL)
        {
        }

        void RenderTargetOGL::reload()
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
