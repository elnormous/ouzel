// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
                case PixelFormat::D: return MTLPixelFormatDepth32Float;
                case PixelFormat::DS: return MTLPixelFormatDepth32Float_Stencil8; // MTLPixelFormatDepth24Unorm_Stencil8 is only available on macOS
                default: return MTLPixelFormatInvalid;
            }
        }

        static MTLTextureType getTextureType(Texture::Dimensions dimensions)
        {
            switch (dimensions)
            {
                case Texture::Dimensions::ONE: return MTLTextureType1D;
                case Texture::Dimensions::TWO: return MTLTextureType2D;
                case Texture::Dimensions::THREE: return MTLTextureType3D;
                case Texture::Dimensions::CUBE: return MTLTextureTypeCube;
                default: throw std::runtime_error("Invalid texture type");
            }
        }

        MetalTexture::MetalTexture(MetalRenderDevice& renderDeviceMetal,
                                   const std::vector<Texture::Level>& levels,
                                   uint32_t initFlags,
                                   uint32_t initSampleCount,
                                   PixelFormat pixelFormat):
            MetalRenderResource(renderDeviceMetal),
            flags(initFlags),
            mipmaps(static_cast<uint32_t>(levels.size())),
            sampleCount(initSampleCount),
            colorFormat(getMetalPixelFormat(pixelFormat)),
            depthFormat(MTLPixelFormatDepth32Float),
            colorBufferLoadAction(MTLLoadActionDontCare),
            depthBufferLoadAction(MTLLoadActionDontCare)
        {
            if ((flags & Texture::RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (colorFormat == MTLPixelFormatInvalid)
                throw std::runtime_error("Invalid pixel format");

            width = static_cast<NSUInteger>(levels.front().size.v[0]);
            height = static_cast<NSUInteger>(levels.front().size.v[1]);

            if (!width || !height)
                throw std::runtime_error("Invalid texture size");

            MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
            textureDescriptor.pixelFormat = colorFormat;
            textureDescriptor.width = width;
            textureDescriptor.height = height;
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.sampleCount = 1;
            textureDescriptor.mipmapLevelCount = static_cast<NSUInteger>(levels.size());

            if (flags & Texture::RENDER_TARGET)
            {
                textureDescriptor.usage = (sampleCount == 1) ? MTLTextureUsageRenderTarget : 0;
                if (flags & Texture::BINDABLE_COLOR_BUFFER) textureDescriptor.usage |= MTLTextureUsageShaderRead;
            }
            else
                textureDescriptor.usage = MTLTextureUsageShaderRead;

            texture = [renderDevice.getDevice() newTextureWithDescriptor:textureDescriptor];

            if (!texture)
                throw std::runtime_error("Failed to create Metal texture");

            if (flags & Texture::RENDER_TARGET)
            {
                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                if (!renderPassDescriptor)
                    throw std::runtime_error("Failed to create Metal render pass descriptor");

                if (sampleCount > 1)
                {
                    MTLTextureDescriptor* msaaTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                    msaaTextureDescriptor.pixelFormat = colorFormat;
                    msaaTextureDescriptor.width = width;
                    msaaTextureDescriptor.height = height;
                    msaaTextureDescriptor.textureType = MTLTextureType2DMultisample;
                    msaaTextureDescriptor.storageMode = MTLStorageModePrivate;
                    msaaTextureDescriptor.sampleCount = sampleCount;
                    msaaTextureDescriptor.mipmapLevelCount = 1;
                    msaaTextureDescriptor.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [renderDevice.getDevice() newTextureWithDescriptor:msaaTextureDescriptor];

                    if (!msaaTexture)
                        throw std::runtime_error("Failed to create MSAA texture");

                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                    renderPassDescriptor.colorAttachments[0].texture = msaaTexture;
                    renderPassDescriptor.colorAttachments[0].resolveTexture = texture;
                }
                else
                {
                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                    renderPassDescriptor.colorAttachments[0].texture = texture;
                }

                if (flags & Texture::DEPTH_BUFFER)
                {
                    MTLTextureDescriptor* depthTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                    depthTextureDescriptor.pixelFormat = depthFormat;
                    depthTextureDescriptor.width = width;
                    depthTextureDescriptor.height = height;
                    //depthTextureDescriptor.textureType = (sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
                    depthTextureDescriptor.textureType = MTLTextureType2D;
                    depthTextureDescriptor.storageMode = MTLStorageModePrivate;
                    //depthTextureDescriptor.sampleCount = sampleCount;
                    depthTextureDescriptor.sampleCount = 1;
                    depthTextureDescriptor.mipmapLevelCount = 1;
                    depthTextureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & Texture::BINDABLE_DEPTH_BUFFER) depthTextureDescriptor.usage |= MTLTextureUsageShaderRead;

                    depthTexture = [renderDevice.getDevice() newTextureWithDescriptor:depthTextureDescriptor];

                    if (!depthTexture)
                        throw std::runtime_error("Failed to create depth texture");

                    renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                }
                else
                    renderPassDescriptor.depthAttachment.texture = nil;

                colorBufferLoadAction = MTLLoadActionClear;
                depthBufferLoadAction = MTLLoadActionDontCare;

                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0F, 0.0F, 0.0F, 0.0F);

                renderPassDescriptor.depthAttachment.clearDepth = 1.0F;
            }
            else
            {
                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(levels[level].size.v[0]),
                                                               static_cast<NSUInteger>(levels[level].size.v[1]))
                                   mipmapLevel:level withBytes:levels[level].data.data()
                                   bytesPerRow:static_cast<NSUInteger>(levels[level].pitch)];
                }
            }

            samplerDescriptor.filter = renderDevice.getTextureFilter();
            samplerDescriptor.addressX = Texture::Address::CLAMP;
            samplerDescriptor.addressY = Texture::Address::CLAMP;
            samplerDescriptor.maxAnisotropy = renderDevice.getMaxAnisotropy();

            updateSamplerState();
        }

        MetalTexture::~MetalTexture()
        {
            if (msaaTexture)
                [msaaTexture release];

            if (renderPassDescriptor)
                [renderPassDescriptor release];

            if (texture)
                [texture release];

            if (samplerState)
                [samplerState release];
        }

        void MetalTexture::setData(const std::vector<Texture::Level>& levels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            if (!(flags & Texture::RENDER_TARGET))
            {
                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(levels[level].size.v[0]),
                                                               static_cast<NSUInteger>(levels[level].size.v[1]))
                                   mipmapLevel:level withBytes:levels[level].data.data()
                                   bytesPerRow:static_cast<NSUInteger>(levels[level].pitch)];
                    }
                }
            }
        }

        void MetalTexture::setFilter(Texture::Filter filter)
        {
            samplerDescriptor.filter = (filter == Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;
            updateSamplerState();
        }

        void MetalTexture::setAddressX(Texture::Address addressX)
        {
            samplerDescriptor.addressX = addressX;
            updateSamplerState();
        }

        void MetalTexture::setAddressY(Texture::Address addressY)
        {
            samplerDescriptor.addressY = addressY;
            updateSamplerState();
        }

        void MetalTexture::setMaxAnisotropy(uint32_t maxAnisotropy)
        {
            samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;
            updateSamplerState();
        }

        void MetalTexture::setClearColorBuffer(bool clear)
        {
            colorBufferLoadAction = clear ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void MetalTexture::setClearDepthBuffer(bool clear)
        {
            depthBufferLoadAction = clear ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void MetalTexture::setClearColor(Color color)
        {
            if (!renderPassDescriptor)
                throw std::runtime_error("Render pass descriptor not initialized");

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(color.normR(),
                                                                                    color.normG(),
                                                                                    color.normB(),
                                                                                    color.normA());
        }

        void MetalTexture::setClearDepth(float newClearDepth)
        {
            if (!renderPassDescriptor)
                throw std::runtime_error("Render pass descriptor not initialized");

            renderPassDescriptor.depthAttachment.clearDepth = newClearDepth;
        }

        void MetalTexture::updateSamplerState()
        {
            if (samplerState) [samplerState release];
            samplerState = renderDevice.getSamplerState(samplerDescriptor);

            if (!samplerState)
                throw std::runtime_error("Failed to get Metal sampler state");

            [samplerState retain];
        }
    } // namespace graphics
} // namespace ouzel

#endif
