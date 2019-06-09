// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <stdexcept>
#include "MetalTexture.hpp"
#include "MetalRenderDevice.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            static MTLPixelFormat getMetalPixelFormat(PixelFormat pixelFormat)
            {
                switch (pixelFormat)
                {
                    case PixelFormat::A8_UNORM: return MTLPixelFormatA8Unorm;
                    case PixelFormat::R8_UNORM: return MTLPixelFormatR8Unorm;
                    case PixelFormat::R8_SNORM: return MTLPixelFormatR8Snorm;
                    case PixelFormat::R8_UINT: return MTLPixelFormatR8Uint;
                    case PixelFormat::R8_SINT: return MTLPixelFormatR8Sint;
                    case PixelFormat::R16_UNORM: return MTLPixelFormatR16Unorm;
                    case PixelFormat::R16_SNORM: return MTLPixelFormatR16Snorm;
                    case PixelFormat::R16_UINT: return MTLPixelFormatR16Uint;
                    case PixelFormat::R16_SINT: return MTLPixelFormatR16Sint;
                    case PixelFormat::R16_FLOAT: return MTLPixelFormatR16Float;
                    case PixelFormat::R32_UINT: return MTLPixelFormatR32Uint;
                    case PixelFormat::R32_SINT: return MTLPixelFormatR32Sint;
                    case PixelFormat::R32_FLOAT: return MTLPixelFormatR32Float;
                    case PixelFormat::RG8_UNORM: return MTLPixelFormatRG8Unorm;
                    case PixelFormat::RG8_SNORM: return MTLPixelFormatRG8Snorm;
                    case PixelFormat::RG8_UINT: return MTLPixelFormatRG8Uint;
                    case PixelFormat::RG8_SINT: return MTLPixelFormatRG8Sint;
                    case PixelFormat::RGBA8_UNORM: return MTLPixelFormatRGBA8Unorm;
                    case PixelFormat::RGBA8_UNORM_SRGB: return MTLPixelFormatRGBA8Unorm_sRGB;
                    case PixelFormat::RGBA8_SNORM: return MTLPixelFormatRGBA8Snorm;
                    case PixelFormat::RGBA8_UINT: return MTLPixelFormatRGBA8Uint;
                    case PixelFormat::RGBA8_SINT: return MTLPixelFormatRGBA8Sint;
                    case PixelFormat::RGBA16_UNORM: return MTLPixelFormatRGBA16Unorm;
                    case PixelFormat::RGBA16_SNORM: return MTLPixelFormatRGBA16Snorm;
                    case PixelFormat::RGBA16_UINT: return MTLPixelFormatRGBA16Uint;
                    case PixelFormat::RGBA16_SINT: return MTLPixelFormatRGBA16Sint;
                    case PixelFormat::RGBA16_FLOAT: return MTLPixelFormatRGBA16Float;
                    case PixelFormat::RGBA32_UINT: return MTLPixelFormatRGBA32Uint;
                    case PixelFormat::RGBA32_SINT: return MTLPixelFormatRGBA32Sint;
                    case PixelFormat::RGBA32_FLOAT: return MTLPixelFormatRGBA32Float;
                    case PixelFormat::DEPTH: return MTLPixelFormatDepth32Float;
                    case PixelFormat::DEPTH_STENCIL: return MTLPixelFormatDepth32Float_Stencil8; // MTLPixelFormatDepth24Unorm_Stencil8 is only available on macOS
                    default: return MTLPixelFormatInvalid;
                }
            }

            static MTLTextureType getTextureType(ouzel::graphics::Texture::Dimensions dimensions, bool multisample)
            {
                if (multisample)
                {
                    switch (dimensions)
                    {
                        case ouzel::graphics::Texture::Dimensions::TWO: return MTLTextureType2DMultisample;
                        default: throw std::runtime_error("Invalid multisample texture type");
                    }
                }
                else
                {
                    switch (dimensions)
                    {
                        case ouzel::graphics::Texture::Dimensions::ONE: return MTLTextureType1D;
                        case ouzel::graphics::Texture::Dimensions::TWO: return MTLTextureType2D;
                        case ouzel::graphics::Texture::Dimensions::THREE: return MTLTextureType3D;
                        case ouzel::graphics::Texture::Dimensions::CUBE: return MTLTextureTypeCube;
                        default: throw std::runtime_error("Invalid texture type");
                    }
                }
            }

            static NSUInteger getCubeFace(ouzel::graphics::Texture::CubeFace face)
            {
                switch (face)
                {
                    case ouzel::graphics::Texture::CubeFace::POSITIVE_X: return 0;
                    case ouzel::graphics::Texture::CubeFace::NEGATIVE_X: return 1;
                    case ouzel::graphics::Texture::CubeFace::POSITIVE_Y: return 2;
                    case ouzel::graphics::Texture::CubeFace::NEGATIVE_Y: return 3;
                    case ouzel::graphics::Texture::CubeFace::POSITIVE_Z: return 4;
                    case ouzel::graphics::Texture::CubeFace::NEGATIVE_Z: return 5;
                    default: throw std::runtime_error("Invalid cube face");
                }
            }

            Texture::Texture(RenderDevice& renderDevice,
                                       const std::vector<ouzel::graphics::Texture::Level>& levels,
                                       ouzel::graphics::Texture::Dimensions dimensions,
                                       uint32_t initFlags,
                                       uint32_t initSampleCount,
                                       PixelFormat initPixelFormat):
                RenderResource(renderDevice),
                flags(initFlags),
                mipmaps(static_cast<uint32_t>(levels.size())),
                sampleCount(initSampleCount),
                pixelFormat(getMetalPixelFormat(initPixelFormat)),
                stencilBuffer(initPixelFormat == PixelFormat::DEPTH_STENCIL)
            {
                if ((flags & ouzel::graphics::Texture::BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                    throw std::runtime_error("Invalid mip map count");

                if (pixelFormat == MTLPixelFormatInvalid)
                    throw std::runtime_error("Invalid pixel format");

                width = static_cast<NSUInteger>(levels.front().size.v[0]);
                height = static_cast<NSUInteger>(levels.front().size.v[1]);

                if (!width || !height)
                    throw std::runtime_error("Invalid texture size");

                // TODO: don't create texture if only MSAA is needed
                MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                textureDescriptor.pixelFormat = pixelFormat;
                textureDescriptor.width = width;
                textureDescriptor.height = height;
                textureDescriptor.textureType = getTextureType(dimensions, false);
                textureDescriptor.sampleCount = 1;
                textureDescriptor.mipmapLevelCount = static_cast<NSUInteger>(levels.size());

                if (initPixelFormat == PixelFormat::DEPTH ||
                    initPixelFormat == PixelFormat::DEPTH_STENCIL)
                    textureDescriptor.storageMode = MTLStorageModePrivate;

                if (flags & ouzel::graphics::Texture::BIND_RENDER_TARGET)
                {
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & ouzel::graphics::Texture::BIND_SHADER &&
                        !(flags & ouzel::graphics::Texture::Flags::BIND_SHADER_MSAA))
                        textureDescriptor.usage |= MTLTextureUsageShaderRead;
                }
                else
                    textureDescriptor.usage = MTLTextureUsageShaderRead;

                texture = [renderDevice.getDevice() newTextureWithDescriptor:textureDescriptor];

                if (!texture)
                    throw std::runtime_error("Failed to create Metal texture");

                if (flags & ouzel::graphics::Texture::BIND_RENDER_TARGET)
                {
                    if (sampleCount > 1)
                    {
                        MTLTextureDescriptor* msaaTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                        msaaTextureDescriptor.pixelFormat = pixelFormat;
                        msaaTextureDescriptor.width = width;
                        msaaTextureDescriptor.height = height;
                        msaaTextureDescriptor.textureType = getTextureType(dimensions, true);
                        msaaTextureDescriptor.storageMode = MTLStorageModePrivate;
                        msaaTextureDescriptor.sampleCount = sampleCount;
                        msaaTextureDescriptor.mipmapLevelCount = 1;
                        msaaTextureDescriptor.usage = MTLTextureUsageRenderTarget;

                        if (flags & ouzel::graphics::Texture::Flags::BIND_SHADER_MSAA)
                            msaaTextureDescriptor.usage |= MTLTextureUsageShaderRead;

                        msaaTexture = [renderDevice.getDevice() newTextureWithDescriptor:msaaTextureDescriptor];

                        if (!msaaTexture)
                            throw std::runtime_error("Failed to create MSAA texture");
                    }
                }
                else
                {
                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].data.empty())
                            [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                                   static_cast<NSUInteger>(levels[level].size.v[0]),
                                                                   static_cast<NSUInteger>(levels[level].size.v[1]))
                                       mipmapLevel:level
                                         withBytes:levels[level].data.data()
                                       bytesPerRow:static_cast<NSUInteger>(levels[level].pitch)];
                    }
                }

                samplerDescriptor.filter = renderDevice.getTextureFilter();
                samplerDescriptor.addressX = ouzel::graphics::Texture::Address::CLAMP_TO_EDGE;
                samplerDescriptor.addressY = ouzel::graphics::Texture::Address::CLAMP_TO_EDGE;
                samplerDescriptor.addressZ = ouzel::graphics::Texture::Address::CLAMP_TO_EDGE;
                samplerDescriptor.maxAnisotropy = renderDevice.getMaxAnisotropy();

                updateSamplerState();
            }

            Texture::~Texture()
            {
                if (msaaTexture)
                    [msaaTexture release];

                if (texture)
                    [texture release];

                if (samplerState)
                    [samplerState release];
            }

            void Texture::setData(const std::vector<ouzel::graphics::Texture::Level>& levels)
            {
                if (!(flags & ouzel::graphics::Texture::DYNAMIC) ||
                    flags & ouzel::graphics::Texture::BIND_RENDER_TARGET)
                    throw std::runtime_error("Texture is not dynamic");

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(levels[level].size.v[0]),
                                                               static_cast<NSUInteger>(levels[level].size.v[1]))
                                   mipmapLevel:level
                                     withBytes:levels[level].data.data()
                                   bytesPerRow:static_cast<NSUInteger>(levels[level].pitch)];
                }
            }

            void Texture::setFilter(ouzel::graphics::Texture::Filter filter)
            {
                samplerDescriptor.filter = (filter == ouzel::graphics::Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;
                updateSamplerState();
            }

            void Texture::setAddressX(ouzel::graphics::Texture::Address addressX)
            {
                samplerDescriptor.addressX = addressX;
                updateSamplerState();
            }

            void Texture::setAddressY(ouzel::graphics::Texture::Address addressY)
            {
                samplerDescriptor.addressY = addressY;
                updateSamplerState();
            }

            void Texture::setAddressZ(ouzel::graphics::Texture::Address addressZ)
            {
                samplerDescriptor.addressZ = addressZ;
                updateSamplerState();
            }

            void Texture::setMaxAnisotropy(uint32_t maxAnisotropy)
            {
                samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;
                updateSamplerState();
            }

            void Texture::updateSamplerState()
            {
                if (samplerState) [samplerState release];
                samplerState = renderDevice.getSamplerState(samplerDescriptor);

                if (!samplerState)
                    throw std::runtime_error("Failed to get Metal sampler state");

                [samplerState retain];
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
