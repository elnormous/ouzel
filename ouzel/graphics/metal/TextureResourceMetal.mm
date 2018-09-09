// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "TextureResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "math/MathUtils.hpp"
#include "utils/Errors.hpp"

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
                default: return MTLPixelFormatInvalid;
            }
        }

        TextureResourceMetal::TextureResourceMetal(RenderDeviceMetal& renderDeviceMetal):
            RenderResource(renderDeviceMetal)
        {
        }

        TextureResourceMetal::~TextureResourceMetal()
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

        void TextureResourceMetal::init(const std::vector<Texture::Level>& newLevels,
                                        uint32_t newFlags,
                                        uint32_t newSampleCount,
                                        PixelFormat newPixelFormat)
        {
            levels = newLevels;
            size = newLevels.front().size;
            flags = newFlags;
            mipmaps = static_cast<uint32_t>(newLevels.size());
            sampleCount = newSampleCount;
            pixelFormat = newPixelFormat;

            if ((flags & Texture::RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            createTexture();

            if (flags & Texture::RENDER_TARGET)
            {
                colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
                depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;

                if (renderPassDescriptor)
                {
                    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                            clearColor.normG(),
                                                                                            clearColor.normB(),
                                                                                            clearColor.normA());

                    renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
                }
            }
            else
            {
                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(levels[level].size.width),
                                                               static_cast<NSUInteger>(levels[level].size.height))
                                   mipmapLevel:level withBytes:levels[level].data.data()
                                   bytesPerRow:static_cast<NSUInteger>(levels[level].pitch)];
                    }
                }
            }

            return updateSamplerState();
        }

        void TextureResourceMetal::setData(const std::vector<Texture::Level>& newLevels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw DataError("Texture is not dynamic");

            levels = newLevels;

            if (!texture)
                createTexture();

            if (!(flags & Texture::RENDER_TARGET))
            {
                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(levels[level].size.width),
                                                               static_cast<NSUInteger>(levels[level].size.height))
                                   mipmapLevel:level withBytes:levels[level].data.data()
                                   bytesPerRow:static_cast<NSUInteger>(levels[level].pitch)];
                    }
                }
            }
        }

        void TextureResourceMetal::setFilter(Texture::Filter newFilter)
        {
            filter = newFilter;

            updateSamplerState();
        }

        void TextureResourceMetal::setAddressX(Texture::Address newAddressX)
        {
            addressX = newAddressX;

            updateSamplerState();
        }

        void TextureResourceMetal::setAddressY(Texture::Address newAddressY)
        {
            addressY = newAddressY;

            updateSamplerState();
        }

        void TextureResourceMetal::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            updateSamplerState();
        }

        void TextureResourceMetal::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void TextureResourceMetal::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void TextureResourceMetal::setClearColor(Color color)
        {
            clearColor = color;

            if (!renderPassDescriptor)
                throw DataError("Render pass descriptor not initialized");

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                    clearColor.normG(),
                                                                                    clearColor.normB(),
                                                                                    clearColor.normA());
        }

        void TextureResourceMetal::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;

            if (!renderPassDescriptor)
                throw DataError("Render pass descriptor not initialized");

            renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
        }

        void TextureResourceMetal::createTexture()
        {
            if (texture)
            {
                [texture release];
                texture = nil;
            }

            if (msaaTexture)
            {
                [msaaTexture release];
                msaaTexture = nil;
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
                renderPassDescriptor = nil;
            }

            if (depthTexture)
            {
                [depthTexture release];
                depthTexture = nil;
            }

            RenderDeviceMetal& renderDeviceMetal = static_cast<RenderDeviceMetal&>(renderDevice);

            width = static_cast<NSUInteger>(size.width);
            height = static_cast<NSUInteger>(size.height);

            if (width > 0 && height > 0)
            {
                MTLPixelFormat metalPixelFormat = getMetalPixelFormat(pixelFormat);

                if (metalPixelFormat == MTLPixelFormatInvalid)
                    throw DataError("Invalid pixel format");

                MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:metalPixelFormat
                                                                                                             width:width
                                                                                                            height:height
                                                                                                         mipmapped:(levels.size() > 1) ? YES : NO];
                textureDescriptor.textureType = MTLTextureType2D;
                textureDescriptor.mipmapLevelCount = static_cast<NSUInteger>(levels.size());

                if (flags & Texture::RENDER_TARGET)
                {
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & Texture::BINDABLE_COLOR_BUFFER) textureDescriptor.usage |= MTLTextureUsageShaderRead;
                }
                else
                    textureDescriptor.usage = MTLTextureUsageShaderRead;

                colorFormat = textureDescriptor.pixelFormat;

                texture = [renderDeviceMetal.getDevice() newTextureWithDescriptor:textureDescriptor];

                if (!texture)
                    throw DataError("Failed to create Metal texture");
            }

            if (flags & Texture::RENDER_TARGET)
            {
                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                if (!renderPassDescriptor)
                    throw DataError("Failed to create Metal render pass descriptor");

                if (sampleCount > 1)
                {
                    MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:static_cast<MTLPixelFormat>(colorFormat)
                                                                                                                 width:width
                                                                                                                height:height
                                                                                                             mipmapped:NO];
                    textureDescriptor.textureType = MTLTextureType2DMultisample;
                    textureDescriptor.storageMode = MTLStorageModePrivate;
                    textureDescriptor.sampleCount = sampleCount;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [renderDeviceMetal.getDevice() newTextureWithDescriptor:textureDescriptor];

                    if (!msaaTexture)
                        throw DataError("Failed to create MSAA texture");

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
                    depthFormat = MTLPixelFormatDepth32Float;

                    MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:static_cast<MTLPixelFormat>(depthFormat)
                                                                                                                 width:width
                                                                                                                height:height
                                                                                                             mipmapped:NO];

                    textureDescriptor.textureType = (sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
                    textureDescriptor.storageMode = MTLStorageModePrivate;
                    textureDescriptor.sampleCount = sampleCount;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & Texture::BINDABLE_DEPTH_BUFFER) textureDescriptor.usage |= MTLTextureUsageShaderRead;

                    depthTexture = [renderDeviceMetal.getDevice() newTextureWithDescriptor:textureDescriptor];

                    if (!depthTexture)
                        throw DataError("Failed to create depth texture");

                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                }
                else
                    renderPassDescriptor.depthAttachment.texture = nil;
            }
        }

        void TextureResourceMetal::updateSamplerState()
        {
            RenderDeviceMetal& renderDeviceMetal = static_cast<RenderDeviceMetal&>(renderDevice);

            RenderDeviceMetal::SamplerStateDescriptor samplerDescriptor;
            samplerDescriptor.filter = (filter == Texture::Filter::DEFAULT) ? renderDeviceMetal.getTextureFilter() : filter;
            samplerDescriptor.addressX = addressX;
            samplerDescriptor.addressY = addressY;
            samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDeviceMetal.getMaxAnisotropy() : maxAnisotropy;

            if (samplerState) [samplerState release];
            samplerState = renderDeviceMetal.getSamplerState(samplerDescriptor);

            if (!samplerState)
                throw DataError("Failed to get Metal sampler state");

            [samplerState retain];
        }
    } // namespace graphics
} // namespace ouzel

#endif
