// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include <stdexcept>
#include "TextureMetal.hpp"
#include "RenderDeviceMetal.hpp"
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
                default: return MTLPixelFormatInvalid;
            }
        }

        TextureMetal::TextureMetal(RenderDeviceMetal& renderDeviceMetal,
                                   const std::vector<Texture::Level>& levels,
                                   uint32_t newFlags,
                                   uint32_t newSampleCount,
                                   PixelFormat newPixelFormat):
            RenderResourceMetal(renderDeviceMetal),
            flags(newFlags),
            mipmaps(static_cast<uint32_t>(levels.size())),
            sampleCount(newSampleCount),
            pixelFormat(newPixelFormat)
        {
            if ((flags & Texture::RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            createTexture(levels);

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

            samplerDescriptor.filter = renderDevice.getTextureFilter();
            samplerDescriptor.addressX = Texture::Address::CLAMP;
            samplerDescriptor.addressY = Texture::Address::CLAMP;
            samplerDescriptor.maxAnisotropy = renderDevice.getMaxAnisotropy();

            updateSamplerState();
        }

        TextureMetal::~TextureMetal()
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

        void TextureMetal::setData(const std::vector<Texture::Level>& levels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            if (!texture)
                createTexture(levels);

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

        void TextureMetal::setFilter(Texture::Filter filter)
        {
            samplerDescriptor.filter = (filter == Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;
            updateSamplerState();
        }

        void TextureMetal::setAddressX(Texture::Address addressX)
        {
            samplerDescriptor.addressX = addressX;
            updateSamplerState();
        }

        void TextureMetal::setAddressY(Texture::Address addressY)
        {
            samplerDescriptor.addressY = addressY;
            updateSamplerState();
        }

        void TextureMetal::setMaxAnisotropy(uint32_t maxAnisotropy)
        {
            samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;
            updateSamplerState();
        }

        void TextureMetal::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void TextureMetal::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
        }

        void TextureMetal::setClearColor(Color color)
        {
            clearColor = color;

            if (!renderPassDescriptor)
                throw std::runtime_error("Render pass descriptor not initialized");

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                    clearColor.normG(),
                                                                                    clearColor.normB(),
                                                                                    clearColor.normA());
        }

        void TextureMetal::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;

            if (!renderPassDescriptor)
                throw std::runtime_error("Render pass descriptor not initialized");

            renderPassDescriptor.depthAttachment.clearDepth = clearDepth;
        }

        void TextureMetal::createTexture(const std::vector<Texture::Level>& levels)
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

            width = static_cast<NSUInteger>(levels.front().size.width);
            height = static_cast<NSUInteger>(levels.front().size.height);

            if (width > 0 && height > 0)
            {
                colorFormat = getMetalPixelFormat(pixelFormat);

                if (colorFormat == MTLPixelFormatInvalid)
                    throw std::runtime_error("Invalid pixel format");

                MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                textureDescriptor.pixelFormat = colorFormat;
                textureDescriptor.width = width;
                textureDescriptor.height = height;
                textureDescriptor.textureType = MTLTextureType2D;
                textureDescriptor.mipmapLevelCount = static_cast<NSUInteger>(levels.size());

                if (flags & Texture::RENDER_TARGET)
                {
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & Texture::BINDABLE_COLOR_BUFFER) textureDescriptor.usage |= MTLTextureUsageShaderRead;
                }
                else
                    textureDescriptor.usage = MTLTextureUsageShaderRead;

                texture = [renderDeviceMetal.getDevice() newTextureWithDescriptor:textureDescriptor];

                if (!texture)
                    throw std::runtime_error("Failed to create Metal texture");
            }

            if (flags & Texture::RENDER_TARGET)
            {
                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                if (!renderPassDescriptor)
                    throw std::runtime_error("Failed to create Metal render pass descriptor");

                if (sampleCount > 1)
                {
                    MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                    textureDescriptor.pixelFormat = colorFormat;
                    textureDescriptor.width = width;
                    textureDescriptor.height = height;
                    textureDescriptor.textureType = MTLTextureType2DMultisample;
                    textureDescriptor.storageMode = MTLStorageModePrivate;
                    textureDescriptor.sampleCount = sampleCount;
                    textureDescriptor.mipmapLevelCount = 1;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [renderDeviceMetal.getDevice() newTextureWithDescriptor:textureDescriptor];

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
                    depthFormat = MTLPixelFormatDepth32Float;

                    MTLTextureDescriptor* textureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
                    textureDescriptor.pixelFormat = depthFormat;
                    textureDescriptor.width = width;
                    textureDescriptor.height = height;
                    textureDescriptor.textureType = (sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
                    textureDescriptor.storageMode = MTLStorageModePrivate;
                    textureDescriptor.sampleCount = sampleCount;
                    textureDescriptor.mipmapLevelCount = 1;
                    textureDescriptor.usage = MTLTextureUsageRenderTarget;
                    if (flags & Texture::BINDABLE_DEPTH_BUFFER) textureDescriptor.usage |= MTLTextureUsageShaderRead;

                    depthTexture = [renderDeviceMetal.getDevice() newTextureWithDescriptor:textureDescriptor];

                    if (!depthTexture)
                        throw std::runtime_error("Failed to create depth texture");

                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                }
                else
                    renderPassDescriptor.depthAttachment.texture = nil;
            }
        }

        void TextureMetal::updateSamplerState()
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
