// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLTEXTURE_HPP
#define OUZEL_GRAPHICS_OGLTEXTURE_HPP

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

#include "graphics/opengl/OGLRenderResource.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLTexture final: public OGLRenderResource
        {
        public:
            OGLTexture(OGLRenderDevice& renderDeviceOGL,
                       const std::vector<Texture::Level>& initLevels,
                       uint32_t initFlags = 0,
                       uint32_t initSampleCount = 1,
                       PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
            ~OGLTexture();

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
            inline float getClearDepth() const { return clearDepth; }
            inline uint32_t getSampleCount() const { return sampleCount; }

            inline GLuint getTextureId() const { return textureId; }

            inline GLuint getFrameBufferId() const { return frameBufferId; }
            inline GLsizei getWidth() const { return width; }
            inline GLsizei getHeight() const { return height; }

            inline GLbitfield getClearMask() const { return clearMask; }
            inline const std::array<GLfloat, 4>& getFrameBufferClearColor() const { return frameBufferClearColor; }

        private:
            void createTexture();
            void setTextureParameters();

            float clearDepth = 1.0F;
            std::vector<Texture::Level> levels;
            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            uint32_t sampleCount = 1;
            Texture::Filter filter = Texture::Filter::DEFAULT;
            Texture::Address addressX = Texture::Address::CLAMP;
            Texture::Address addressY = Texture::Address::CLAMP;
            uint32_t maxAnisotropy = 0;

            GLuint textureId = 0;

            GLsizei width = 0;
            GLsizei height = 0;
            GLenum internalPixelFormat = GL_NONE;
            GLenum pixelFormat = GL_NONE;
            GLenum pixelType = GL_NONE;

            GLuint frameBufferId = 0;
            GLuint colorBufferId = 0;
            GLuint depthTextureId = 0;
            GLuint depthBufferId = 0;

            GLbitfield clearMask = 0;
            std::array<GLfloat, 4> frameBufferClearColor{{0.0F, 0.0F, 0.0F, 0.0F}};
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLTEXTURE_HPP
