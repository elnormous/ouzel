// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "RenderTargetResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetResourceOGL::RenderTargetResourceOGL(RenderDeviceOGL& renderDeviceOGL):
            RenderResourceOGL(renderDeviceOGL)
        {
        }

        void RenderTargetResourceOGL::reload()
        {
        }
    } // namespace graphics
} // namespace ouzel

#endif
