// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#  define GL_GLEXT_PROTOTYPES 1
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  define GL_GLEXT_PROTOTYPES 1
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "graphics/TextureResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class TextureResourceOGL: public TextureResource
        {
        public:
            explicit TextureResourceOGL(RenderDeviceOGL& renderDeviceOGL);
            virtual ~TextureResourceOGL();

            void init(const std::vector<Texture::Level>& newLevels,
                      uint32_t newFlags = 0,
                      uint32_t newSampleCount = 1,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            virtual void reload() override;

            void setData(const std::vector<Texture::Level>& newLevels);
            void setFilter(Texture::Filter newFilter);
            void setAddressX(Texture::Address newAddressX);
            void setAddressY(Texture::Address newAddressY);
            void setMaxAnisotropy(uint32_t newMaxAnisotropy);
            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

            inline GLuint getTextureId() const { return textureId; }

            inline GLuint getFrameBufferId() const { return frameBufferId; }
            inline GLsizei getWidth() const { return width; }
            inline GLsizei getHeight() const { return height; }

            inline GLbitfield getClearMask() const { return clearMask; }
            inline const GLfloat* getFrameBufferClearColor() const { return frameBufferClearColor; }

        private:
            void createTexture();
            void setTextureParameters();

            GLuint textureId = 0;

            GLsizei width = 0;
            GLsizei height = 0;
            GLenum oglInternalPixelFormat = GL_NONE;
            GLenum oglPixelFormat = GL_NONE;
            GLenum oglPixelType = GL_NONE;

            GLuint frameBufferId = 0;
            GLuint colorBufferId = 0;
            GLuint depthTextureId = 0;
            GLuint depthBufferId = 0;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];
        };
    } // namespace graphics
} // namespace ouzel

#endif
