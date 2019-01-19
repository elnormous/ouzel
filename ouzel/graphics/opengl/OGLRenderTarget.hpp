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

#include <set>
#include "graphics/opengl/OGLRenderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;
        class OGLTexture;

        class OGLRenderTarget final: public OGLRenderResource
        {
        public:
            explicit OGLRenderTarget(OGLRenderDevice& renderDeviceOGL);
            ~OGLRenderTarget();

            void reload() override;

            void addColorTexture(OGLTexture* texture);
            void removeColorTexture(OGLTexture* texture);
            void setDepthTexture(OGLTexture* texture);

            inline GLuint getFrameBufferId() const { return frameBufferId; }

            inline GLbitfield getClearMask() const { return clearMask; }
            inline const std::array<GLfloat, 4>& getFrameBufferClearColor() const { return frameBufferClearColor; }
            inline float getClearDepth() const { return clearDepth; }

        private:
            GLuint frameBufferId = 0;

            std::set<OGLTexture*> colorTextures;
            OGLTexture* depthTexture = nullptr;

            GLbitfield clearMask = 0;
            std::array<GLfloat, 4> frameBufferClearColor{{0.0F, 0.0F, 0.0F, 0.0F}};
            float clearDepth = 1.0F;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERTARGET_HPP
