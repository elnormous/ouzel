// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLTEXTURE_HPP
#define OUZEL_GRAPHICS_OGLTEXTURE_HPP

#include <array>
#include <utility>

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "OGL.h"

#if OUZEL_OPENGLES
#  include "GLES/gl.h"
#  include "GLES2/gl2.h"
#  include "GLES2/gl2ext.h"
#  include "GLES3/gl3.h"
#else
#  include "GL/glcorearb.h"
#  include "GL/glext.h"
#endif

#include "OGLRenderResource.hpp"
#include "../Flags.hpp"
#include "../PixelFormat.hpp"
#include "../SamplerAddressMode.hpp"
#include "../SamplerFilter.hpp"
#include "../TextureType.hpp"
#include "../../math/Size.hpp"

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
                Texture(RenderDevice& initRenderDevice,
                        const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& initLevels,
                        TextureType type,
                        Flags initFlags = Flags::none,
                        std::uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8UNorm);
                ~Texture() override;

                void reload() final;

                void setData(const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& newLevels);
                void setFilter(SamplerFilter newFilter);
                void setAddressX(SamplerAddressMode newAddressX);
                void setAddressY(SamplerAddressMode newAddressY);
                void setAddressZ(SamplerAddressMode newAddressZ);
                void setMaxAnisotropy(std::uint32_t newMaxAnisotropy);

                auto getFlags() const noexcept { return flags; }
                auto getMipmaps() const noexcept { return mipmaps; }

                auto getFilter() const noexcept { return filter; }
                auto getAddressX() const noexcept { return addressX; }
                auto getAddressY() const noexcept { return addressY; }
                auto getMaxAnisotropy() const noexcept { return maxAnisotropy; }
                auto getSampleCount() const noexcept { return sampleCount; }

                auto getTextureId() const noexcept { return textureId; }
                auto getBufferId() const noexcept { return bufferId; }

                auto getWidth() const noexcept { return width; }
                auto getHeight() const noexcept { return height; }

                auto getPixelFormat() const noexcept { return pixelFormat; }

            private:
                void createTexture();
                void setTextureParameters();

                std::vector<std::pair<Size2U, std::vector<std::uint8_t>>> levels;
                Flags flags = Flags::none;
                std::uint32_t mipmaps = 0;
                std::uint32_t sampleCount = 1;
                SamplerFilter filter = SamplerFilter::Default;
                SamplerAddressMode addressX = SamplerAddressMode::clampToEdge;
                SamplerAddressMode addressY = SamplerAddressMode::clampToEdge;
                SamplerAddressMode addressZ = SamplerAddressMode::clampToEdge;
                std::uint32_t maxAnisotropy = 0;

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
