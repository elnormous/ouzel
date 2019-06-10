// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLTEXTURE_HPP
#define OUZEL_GRAPHICS_OGLTEXTURE_HPP

#include <array>

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGL.h"

#if OUZEL_OPENGLES
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
        namespace opengl
        {
            class RenderDevice;

            class Texture final: public RenderResource
            {
            public:
                Texture(RenderDevice& renderDevice,
                        const std::vector<ouzel::graphics::Texture::Level>& initLevels,
                        ouzel::graphics::Texture::Dimensions dimensions,
                        uint32_t initFlags = 0,
                        uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
                ~Texture();

                void reload() final;

                void setData(const std::vector<ouzel::graphics::Texture::Level>& newLevels);
                void setFilter(ouzel::graphics::Texture::Filter newFilter);
                void setAddressX(ouzel::graphics::Texture::Address newAddressX);
                void setAddressY(ouzel::graphics::Texture::Address newAddressY);
                void setAddressZ(ouzel::graphics::Texture::Address newAddressZ);
                void setMaxAnisotropy(uint32_t newMaxAnisotropy);

                inline uint32_t getFlags() const { return flags; }
                inline uint32_t getMipmaps() const { return mipmaps; }

                inline ouzel::graphics::Texture::Filter getFilter() const { return filter; }
                inline ouzel::graphics::Texture::Address getAddressX() const { return addressX; }
                inline ouzel::graphics::Texture::Address getAddressY() const { return addressY; }
                inline uint32_t getMaxAnisotropy() const { return maxAnisotropy; }
                inline uint32_t getSampleCount() const { return sampleCount; }

                inline GLuint getTextureId() const { return textureId; }
                inline GLuint getBufferId() const { return bufferId; }

                inline GLsizei getWidth() const { return width; }
                inline GLsizei getHeight() const { return height; }

                inline GLenum getPixelFormat() const { return pixelFormat; }

            private:
                void createTexture();
                void setTextureParameters();

                std::vector<ouzel::graphics::Texture::Level> levels;
                uint32_t flags = 0;
                uint32_t mipmaps = 0;
                uint32_t sampleCount = 1;
                ouzel::graphics::Texture::Filter filter = ouzel::graphics::Texture::Filter::DEFAULT;
                ouzel::graphics::Texture::Address addressX = ouzel::graphics::Texture::Address::CLAMP_TO_EDGE;
                ouzel::graphics::Texture::Address addressY = ouzel::graphics::Texture::Address::CLAMP_TO_EDGE;
                ouzel::graphics::Texture::Address addressZ = ouzel::graphics::Texture::Address::CLAMP_TO_EDGE;
                uint32_t maxAnisotropy = 0;

                GLenum textureTarget = 0;
                GLuint textureId = 0;
                GLuint bufferId = 0;

                GLsizei width = 0;
                GLsizei height = 0;
                GLenum internalPixelFormat = GL_NONE;
                GLenum pixelFormat = GL_NONE;
                GLenum pixelType = GL_NONE;
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLTEXTURE_HPP
