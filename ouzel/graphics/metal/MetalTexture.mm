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
                    case PixelFormat::A8UNorm: return MTLPixelFormatA8Unorm;
                    case PixelFormat::R8UNorm: return MTLPixelFormatR8Unorm;
                    case PixelFormat::R8SNorm: return MTLPixelFormatR8Snorm;
                    case PixelFormat::R8UInt: return MTLPixelFormatR8Uint;
                    case PixelFormat::R8SInt: return MTLPixelFormatR8Sint;
                    case PixelFormat::R16UNorm: return MTLPixelFormatR16Unorm;
                    case PixelFormat::R16SNorm: return MTLPixelFormatR16Snorm;
                    case PixelFormat::R16UInt: return MTLPixelFormatR16Uint;
                    case PixelFormat::R16SInt: return MTLPixelFormatR16Sint;
                    case PixelFormat::R16Float: return MTLPixelFormatR16Float;
                    case PixelFormat::R32UInt: return MTLPixelFormatR32Uint;
                    case PixelFormat::R32SInt: return MTLPixelFormatR32Sint;
                    case PixelFormat::R32Float: return MTLPixelFormatR32Float;
                    case PixelFormat::RG8UNorm: return MTLPixelFormatRG8Unorm;
                    case PixelFormat::RG8SNorm: return MTLPixelFormatRG8Snorm;
                    case PixelFormat::RG8UInt: return MTLPixelFormatRG8Uint;
                    case PixelFormat::RG8SInt: return MTLPixelFormatRG8Sint;
                    case PixelFormat::RGBA8UNorm: return MTLPixelFormatRGBA8Unorm;
                    case PixelFormat::RGBA8UNormSRGB: return MTLPixelFormatRGBA8Unorm_sRGB;
                    case PixelFormat::RGBA8SNorm: return MTLPixelFormatRGBA8Snorm;
                    case PixelFormat::RGBA8UInt: return MTLPixelFormatRGBA8Uint;
                    case PixelFormat::RGBA8SInt: return MTLPixelFormatRGBA8Sint;
                    case PixelFormat::RGBA16UNorm: return MTLPixelFormatRGBA16Unorm;
                    case PixelFormat::RGBA16SNorm: return MTLPixelFormatRGBA16Snorm;
                    case PixelFormat::RGBA16UInt: return MTLPixelFormatRGBA16Uint;
                    case PixelFormat::RGBA16SInt: return MTLPixelFormatRGBA16Sint;
                    case PixelFormat::RGBA16Float: return MTLPixelFormatRGBA16Float;
                    case PixelFormat::RGBA32UInt: return MTLPixelFormatRGBA32Uint;
                    case PixelFormat::RGBA32SInt: return MTLPixelFormatRGBA32Sint;
                    case PixelFormat::RGBA32Float: return MTLPixelFormatRGBA32Float;
                    case PixelFormat::Depth: return MTLPixelFormatDepth32Float;
                    case PixelFormat::DepthStencil: return MTLPixelFormatDepth32Float_Stencil8; // MTLPixelFormatDepth24Unorm_Stencil8 is only available on macOS
                    default: return MTLPixelFormatInvalid;
                }
            }

            static MTLTextureType getTextureType(TextureType type, bool multisample)
            {
                if (multisample)
                {
                    switch (type)
                    {
                        case TextureType::TwoDimensional: return MTLTextureType2DMultisample;
                        default: throw std::runtime_error("Invalid multisample texture type");
                    }
                }
                else
                {
                    switch (type)
                    {
                        case TextureType::OneDimensional: return MTLTextureType1D;
                        case TextureType::TwoDimensional: return MTLTextureType2D;
                        case TextureType::ThreeDimensional: return MTLTextureType3D;
                        case TextureType::Cube: return MTLTextureTypeCube;
                        default: throw std::runtime_error("Invalid texture type");
                    }
                }
            }

            static NSUInteger getCubeFace(CubeFace face)
            {
                switch (face)
                {
                    case CubeFace::PositiveX: return 0;
                    case CubeFace::NegativeX: return 1;
                    case CubeFace::PositiveY: return 2;
                    case CubeFace::NegativeY: return 3;
                    case CubeFace::PositiveZ: return 4;
                    case CubeFace::NegativeZ: return 5;
                    default: throw std::runtime_error("Invalid cube face");
                }
            }

            Texture::Texture(RenderDevice& initRenderDevice,
                             const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& levels,
                             TextureType type,
                             uint32_t initFlags,
                             uint32_t initSampleCount,
                             PixelFormat initPixelFormat):
                RenderResource(initRenderDevice),
                flags(initFlags),
                mipmaps(static_cast<uint32_t>(levels.size())),
                sampleCount(initSampleCount),
                pixelFormat(getMetalPixelFormat(initPixelFormat)),
                pixelSize(getPixelSize(initPixelFormat)),
                stencilBuffer(initPixelFormat == PixelFormat::DepthStencil)
            {
                if ((flags & Flags::BindRenderTarget) && (mipmaps == 0 || mipmaps > 1))
                    throw std::runtime_error("Invalid mip map count");

                if (pixelFormat == MTLPixelFormatInvalid)
                    throw std::runtime_error("Invalid pixel format");

                width = static_cast<NSUInteger>(levels.front().first.v[0]);
                height = static_cast<NSUInteger>(levels.front().first.v[1]);

                if (!width || !height)
                    throw std::runtime_error("Invalid texture size");

                // TODO: don't create texture if only MSAA is needed
                MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                textureDescriptor.pixelFormat = pixelFormat;
                textureDescriptor.width = width;
                textureDescriptor.height = height;
                textureDescriptor.textureType = getTextureType(type, false);
                textureDescriptor.sampleCount = 1;
                textureDescriptor.mipmapLevelCount = static_cast<NSUInteger>(levels.size());

                if (initPixelFormat == PixelFormat::Depth ||
                    initPixelFormat == PixelFormat::DepthStencil)
                    textureDescriptor.storageMode = MTLStorageModePrivate;

                if (flags & Flags::BindRenderTarget)
                {
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & Flags::BindShader &&
                        !(flags & Flags::BindShaderMsaa))
                        textureDescriptor.usage |= MTLTextureUsageShaderRead;
                }
                else
                    textureDescriptor.usage = MTLTextureUsageShaderRead;

                texture = [renderDevice.getDevice() newTextureWithDescriptor:textureDescriptor];

                if (!texture)
                    throw std::runtime_error("Failed to create Metal texture");

                if (flags & Flags::BindRenderTarget)
                {
                    if (sampleCount > 1)
                    {
                        MTLTextureDescriptor* msaaTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                        msaaTextureDescriptor.pixelFormat = pixelFormat;
                        msaaTextureDescriptor.width = width;
                        msaaTextureDescriptor.height = height;
                        msaaTextureDescriptor.textureType = getTextureType(type, true);
                        msaaTextureDescriptor.storageMode = MTLStorageModePrivate;
                        msaaTextureDescriptor.sampleCount = sampleCount;
                        msaaTextureDescriptor.mipmapLevelCount = 1;
                        msaaTextureDescriptor.usage = MTLTextureUsageRenderTarget;

                        if (flags & Flags::BindShaderMsaa)
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
                        if (!levels[level].second.empty())
                            [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                                   static_cast<NSUInteger>(levels[level].first.v[0]),
                                                                   static_cast<NSUInteger>(levels[level].first.v[1]))
                                       mipmapLevel:level
                                         withBytes:levels[level].second.data()
                                       bytesPerRow:static_cast<NSUInteger>(levels[level].first.v[0] * pixelSize)];
                    }
                }

                samplerDescriptor.filter = renderDevice.getTextureFilter();
                samplerDescriptor.addressX = SamplerAddressMode::ClampToEdge;
                samplerDescriptor.addressY = SamplerAddressMode::ClampToEdge;
                samplerDescriptor.addressZ = SamplerAddressMode::ClampToEdge;
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

            void Texture::setData(const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& levels)
            {
                if (!(flags & Flags::Dynamic) ||
                    flags & Flags::BindRenderTarget)
                    throw std::runtime_error("Texture is not dynamic");

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].second.empty())
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(levels[level].first.v[0]),
                                                               static_cast<NSUInteger>(levels[level].first.v[1]))
                                   mipmapLevel:level
                                     withBytes:levels[level].second.data()
                                   bytesPerRow:static_cast<NSUInteger>(levels[level].first.v[0] * pixelSize)];
                }
            }

            void Texture::setFilter(SamplerFilter filter)
            {
                samplerDescriptor.filter = filter;
                updateSamplerState();
            }

            void Texture::setAddressX(SamplerAddressMode addressX)
            {
                samplerDescriptor.addressX = addressX;
                updateSamplerState();
            }

            void Texture::setAddressY(SamplerAddressMode addressY)
            {
                samplerDescriptor.addressY = addressY;
                updateSamplerState();
            }

            void Texture::setAddressZ(SamplerAddressMode addressZ)
            {
                samplerDescriptor.addressZ = addressZ;
                updateSamplerState();
            }

            void Texture::setMaxAnisotropy(uint32_t maxAnisotropy)
            {
                samplerDescriptor.maxAnisotropy = maxAnisotropy;
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
