// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
#define OUZEL_GRAPHICS_OGLRENDERTARGET_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/opengl/OGLRenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLRenderTarget final: public OGLRenderResource
        {
        public:
            explicit OGLRenderTarget(OGLRenderDevice& renderDeviceOGL);
            ~OGLRenderTarget();

            void reload() override;

            inline GLuint getFrameBufferId() const { return frameBufferId; }

        private:
            GLuint frameBufferId = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
