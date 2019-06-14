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
                        const std::vector<graphics::Texture::Level>& initLevels,
                        TextureType type,
                        uint32_t initFlags = 0,
                        uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
                ~Texture();

                void reload() final;

                void setData(const std::vector<graphics::Texture::Level>& newLevels);
                void setFilter(SamplerFilter newFilter);
                void setAddressX(SamplerAddressMode newAddressX);
                void setAddressY(SamplerAddressMode newAddressY);
                void setAddressZ(SamplerAddressMode newAddressZ);
                void setMaxAnisotropy(uint32_t newMaxAnisotropy);

                inline uint32_t getFlags() const { return flags; }
                inline uint32_t getMipmaps() const { return mipmaps; }

                inline SamplerFilter getFilter() const { return filter; }
                inline SamplerAddressMode getAddressX() const { return addressX; }
                inline SamplerAddressMode getAddressY() const { return addressY; }
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

                std::vector<graphics::Texture::Level> levels;
                uint32_t flags = 0;
                uint32_t mipmaps = 0;
                uint32_t sampleCount = 1;
                SamplerFilter filter = SamplerFilter::DEFAULT;
                SamplerAddressMode addressX = SamplerAddressMode::CLAMP_TO_EDGE;
                SamplerAddressMode addressY = SamplerAddressMode::CLAMP_TO_EDGE;
                SamplerAddressMode addressZ = SamplerAddressMode::CLAMP_TO_EDGE;
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
