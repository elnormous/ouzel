// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            explicit TextureResourceOGL(RenderDeviceOGL& initRenderDeviceOGL);
            virtual ~TextureResourceOGL();

            virtual bool init(const Size2& newSize,
                              uint32_t newFlags = 0,
                              uint32_t newMipmaps = 0,
                              uint32_t newSampleCount = 1,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;
            virtual bool init(const std::vector<uint8_t>& newData,
                              const Size2& newSize,
                              uint32_t newFlags = 0,
                              uint32_t newMipmaps = 0,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;
            virtual bool init(const std::vector<Texture::Level>& newLevels,
                              const Size2& newSize,
                              uint32_t newFlags = 0,
                              PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM) override;

            virtual bool reload() override;

            virtual bool setSize(const Size2& newSize) override;
            virtual bool setData(const std::vector<uint8_t>& newData, const Size2& newSize) override;
            virtual bool setFilter(Texture::Filter newFilter) override;
            virtual bool setAddressX(Texture::Address newAddressX) override;
            virtual bool setAddressY(Texture::Address newAddressY) override;
            virtual bool setMaxAnisotropy(uint32_t newMaxAnisotropy) override;
            virtual bool setClearColorBuffer(bool clear) override;
            virtual bool setClearDepthBuffer(bool clear) override;
            virtual bool setClearColor(Color color) override;

            inline GLuint getTextureId() const { return textureId; }

            inline GLuint getFrameBufferId() const { return frameBufferId; }
            inline GLsizei getWidth() const { return width; }
            inline GLsizei getHeight() const { return height; }

            inline GLbitfield getClearMask() const { return clearMask; }
            inline const GLfloat* getFrameBufferClearColor() const { return frameBufferClearColor; }

        private:
            bool createTexture();
            bool setTextureParameters();

            RenderDeviceOGL& renderDeviceOGL;

            GLuint textureId = 0;

            GLsizei width = 0;
            GLsizei height = 0;
            GLint oglInternalPixelFormat = GL_NONE;
            GLenum oglPixelFormat = GL_NONE;
            GLenum oglPixelType = GL_NONE;

            GLuint frameBufferId = 0;
            GLuint depthBufferId = 0;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];
        };
    } // namespace graphics
} // namespace ouzel

#endif
