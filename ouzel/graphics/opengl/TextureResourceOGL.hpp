// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_TEXTURERESOURCEOGL_HPP
#define OUZEL_GRAPHICS_TEXTURERESOURCEOGL_HPP

#include <array>

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

#include "graphics/opengl/RenderResourceOGL.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class TextureResourceOGL final: public RenderResourceOGL
        {
        public:
            TextureResourceOGL(RenderDeviceOGL& renderDeviceOGL,
                               const std::vector<Texture::Level>& newLevels,
                               uint32_t newFlags = 0,
                               uint32_t newSampleCount = 1,
                               PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            ~TextureResourceOGL();

            void reload() override;

            void setData(const std::vector<Texture::Level>& newLevels);
            void setFilter(Texture::Filter newFilter);
            void setAddressX(Texture::Address newAddressX);
            void setAddressY(Texture::Address newAddressY);
            void setMaxAnisotropy(uint32_t newMaxAnisotropy);
            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getMipmaps() const { return mipmaps; }

            inline Texture::Filter getFilter() const { return filter; }
            inline Texture::Address getAddressX() const { return addressX; }
            inline Texture::Address getAddressY() const { return addressY; }
            inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            inline Color getClearColor() const { return clearColor; }
            inline float getClearDepth() const { return clearDepth; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline PixelFormat getPixelFormat() const { return pixelFormat; }

            inline GLuint getTextureId() const { return textureId; }

            inline GLuint getFrameBufferId() const { return frameBufferId; }
            inline GLsizei getWidth() const { return width; }
            inline GLsizei getHeight() const { return height; }

            inline GLbitfield getClearMask() const { return clearMask; }
            inline const std::array<GLfloat, 4>& getFrameBufferClearColor() const { return frameBufferClearColor; }

        private:
            void createTexture();
            void setTextureParameters();

            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
            std::vector<Texture::Level> levels;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            Texture::Filter filter = Texture::Filter::DEFAULT;
            Texture::Address addressX = Texture::Address::CLAMP;
            Texture::Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;

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
            std::array<GLfloat, 4> frameBufferClearColor;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_TEXTURERESOURCEOGL_HPP
