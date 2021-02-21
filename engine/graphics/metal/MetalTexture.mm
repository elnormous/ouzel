// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalTexture.hpp"
#include "MetalError.hpp"
#include "MetalRenderDevice.hpp"
#include "../../math/MathUtils.hpp"

namespace ouzel::graphics::metal
{
    namespace
    {
        constexpr MTLPixelFormat getMetalPixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::a8UnsignedNorm: return MTLPixelFormatA8Unorm;
                case PixelFormat::r8UnsignedNorm: return MTLPixelFormatR8Unorm;
                case PixelFormat::r8SignedNorm: return MTLPixelFormatR8Snorm;
                case PixelFormat::r8UnsignedInt: return MTLPixelFormatR8Uint;
                case PixelFormat::r8SignedInt: return MTLPixelFormatR8Sint;
                case PixelFormat::r16UnsignedNorm: return MTLPixelFormatR16Unorm;
                case PixelFormat::r16SignedNorm: return MTLPixelFormatR16Snorm;
                case PixelFormat::r16UnsignedInt: return MTLPixelFormatR16Uint;
                case PixelFormat::r16SignedInt: return MTLPixelFormatR16Sint;
                case PixelFormat::r16Float: return MTLPixelFormatR16Float;
                case PixelFormat::r32UnsignedInt: return MTLPixelFormatR32Uint;
                case PixelFormat::r32SignedInt: return MTLPixelFormatR32Sint;
                case PixelFormat::r32Float: return MTLPixelFormatR32Float;
                case PixelFormat::rg8UnsignedNorm: return MTLPixelFormatRG8Unorm;
                case PixelFormat::rg8SignedNorm: return MTLPixelFormatRG8Snorm;
                case PixelFormat::rg8UnsignedInt: return MTLPixelFormatRG8Uint;
                case PixelFormat::rg8SignedInt: return MTLPixelFormatRG8Sint;
                case PixelFormat::rgba8UnsignedNorm: return MTLPixelFormatRGBA8Unorm;
                case PixelFormat::rgba8UnsignedNormSRGB: return MTLPixelFormatRGBA8Unorm_sRGB;
                case PixelFormat::rgba8SignedNorm: return MTLPixelFormatRGBA8Snorm;
                case PixelFormat::rgba8UnsignedInt: return MTLPixelFormatRGBA8Uint;
                case PixelFormat::rgba8SignedInt: return MTLPixelFormatRGBA8Sint;
                case PixelFormat::rgba16UnsignedNorm: return MTLPixelFormatRGBA16Unorm;
                case PixelFormat::rgba16SignedNorm: return MTLPixelFormatRGBA16Snorm;
                case PixelFormat::rgba16UnsignedInt: return MTLPixelFormatRGBA16Uint;
                case PixelFormat::rgba16SignedInt: return MTLPixelFormatRGBA16Sint;
                case PixelFormat::rgba16Float: return MTLPixelFormatRGBA16Float;
                case PixelFormat::rgba32UnsignedInt: return MTLPixelFormatRGBA32Uint;
                case PixelFormat::rgba32SignedInt: return MTLPixelFormatRGBA32Sint;
                case PixelFormat::rgba32Float: return MTLPixelFormatRGBA32Float;
                case PixelFormat::depth: return MTLPixelFormatDepth32Float;
                case PixelFormat::depthStencil: return MTLPixelFormatDepth32Float_Stencil8; // MTLPixelFormatDepth24Unorm_Stencil8 is only available on macOS
                default: throw std::runtime_error("Invalid pixel format");
            }
        }

        constexpr MTLTextureType getTextureType(TextureType type, bool multisample)
        {
            if (multisample)
            {
                switch (type)
                {
                    case TextureType::twoDimensional: return MTLTextureType2DMultisample;
                    default: throw Error("Invalid multisample texture type");
                }
            }
            else
            {
                switch (type)
                {
                    case TextureType::oneDimensional: return MTLTextureType1D;
                    case TextureType::twoDimensional: return MTLTextureType2D;
                    case TextureType::threeDimensional: return MTLTextureType3D;
                    case TextureType::cube: return MTLTextureTypeCube;
                    default: throw Error("Invalid texture type");
                }
            }
        }

        [[maybe_unused]]
        constexpr NSUInteger getCubeFace(CubeFace face)
        {
            switch (face)
            {
                case CubeFace::positiveX: return 0;
                case CubeFace::negativeX: return 1;
                case CubeFace::positiveY: return 2;
                case CubeFace::negativeY: return 3;
                case CubeFace::positiveZ: return 4;
                case CubeFace::negativeZ: return 5;
                default: throw Error("Invalid cube face");
            }
        }
    }

    Texture::Texture(RenderDevice& initRenderDevice,
                     const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels,
                     TextureType type,
                     Flags initFlags,
                     std::uint32_t initSampleCount,
                     PixelFormat initPixelFormat,
                     SamplerFilter initFilter,
                     std::uint32_t initMaxAnisotropy):
        RenderResource(initRenderDevice),
        flags(initFlags),
        mipmaps(static_cast<std::uint32_t>(levels.size())),
        sampleCount(initSampleCount),
        pixelFormat(getMetalPixelFormat(initPixelFormat)),
        pixelSize(getPixelSize(initPixelFormat)),
        stencilBuffer(initPixelFormat == PixelFormat::depthStencil)
    {
        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget &&
            (mipmaps == 0 || mipmaps > 1))
            throw Error("Invalid mip map count");

        if (pixelFormat == MTLPixelFormatInvalid)
            throw Error("Invalid pixel format");

        width = static_cast<NSUInteger>(levels.front().first.v[0]);
        height = static_cast<NSUInteger>(levels.front().first.v[1]);

        if (!width || !height)
            throw Error("Invalid texture size");

        // TODO: don't create texture if only MSAA is needed
        Pointer<MTLTextureDescriptor*> textureDescriptor = [[MTLTextureDescriptor alloc] init];
        textureDescriptor.get().pixelFormat = pixelFormat;
        textureDescriptor.get().width = width;
        textureDescriptor.get().height = height;
        textureDescriptor.get().textureType = getTextureType(type, false);
        textureDescriptor.get().sampleCount = 1;
        textureDescriptor.get().mipmapLevelCount = static_cast<NSUInteger>(levels.size());

        if (initPixelFormat == PixelFormat::depth ||
            initPixelFormat == PixelFormat::depthStencil)
            textureDescriptor.get().storageMode = MTLStorageModePrivate;

        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
        {
            textureDescriptor.get().usage = MTLTextureUsageRenderTarget;
            if ((flags & Flags::bindShader) == Flags::bindShader &&
                (flags & Flags::bindShaderMsaa) != Flags::bindShaderMsaa)
                textureDescriptor.get().usage |= MTLTextureUsageShaderRead;
        }
        else
            textureDescriptor.get().usage = MTLTextureUsageShaderRead;

        texture = [renderDevice.getDevice().get() newTextureWithDescriptor:textureDescriptor.get()];

        if (!texture)
            throw Error("Failed to create Metal texture");

        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
        {
            if (sampleCount > 1)
            {
                Pointer<MTLTextureDescriptor*> msaaTextureDescriptor = [[MTLTextureDescriptor alloc] init];
                msaaTextureDescriptor.get().pixelFormat = pixelFormat;
                msaaTextureDescriptor.get().width = width;
                msaaTextureDescriptor.get().height = height;
                msaaTextureDescriptor.get().textureType = getTextureType(type, true);
                msaaTextureDescriptor.get().storageMode = MTLStorageModePrivate;
                msaaTextureDescriptor.get().sampleCount = sampleCount;
                msaaTextureDescriptor.get().mipmapLevelCount = 1;
                msaaTextureDescriptor.get().usage = MTLTextureUsageRenderTarget;

                if ((flags & Flags::bindShaderMsaa) == Flags::bindShaderMsaa)
                    msaaTextureDescriptor.get().usage |= MTLTextureUsageShaderRead;

                msaaTexture = [renderDevice.getDevice().get() newTextureWithDescriptor:msaaTextureDescriptor.get()];

                if (!msaaTexture)
                    throw Error("Failed to create MSAA texture");
            }
        }
        else
        {
            for (std::size_t level = 0; level < levels.size(); ++level)
            {
                if (!levels[level].second.empty())
                    [texture.get() replaceRegion:MTLRegionMake2D(0, 0,
                                                                 static_cast<NSUInteger>(levels[level].first.v[0]),
                                                                 static_cast<NSUInteger>(levels[level].first.v[1]))
                                     mipmapLevel:level
                                       withBytes:levels[level].second.data()
                                     bytesPerRow:static_cast<NSUInteger>(levels[level].first.v[0] * pixelSize)];
            }
        }

        samplerDescriptor.filter = initFilter;
        samplerDescriptor.addressX = SamplerAddressMode::clampToEdge;
        samplerDescriptor.addressY = SamplerAddressMode::clampToEdge;
        samplerDescriptor.addressZ = SamplerAddressMode::clampToEdge;
        samplerDescriptor.maxAnisotropy = initMaxAnisotropy;

        updateSamplerState();
    }

    void Texture::setData(const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic ||
            (flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
            throw Error("Texture is not dynamic");

        for (std::size_t level = 0; level < levels.size(); ++level)
        {
            if (!levels[level].second.empty())
                [texture.get() replaceRegion:MTLRegionMake2D(0, 0,
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

    void Texture::setMaxAnisotropy(std::uint32_t maxAnisotropy)
    {
        samplerDescriptor.maxAnisotropy = maxAnisotropy;
        updateSamplerState();
    }

    void Texture::updateSamplerState()
    {
        samplerState = renderDevice.getSamplerState(samplerDescriptor);

        if (!samplerState)
            throw Error("Failed to get Metal sampler state");
    }
}

#endif
