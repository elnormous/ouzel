// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "TextureResourceMetal.hpp"
#include "RenderDeviceMetal.hpp"
#include "math/MathUtils.hpp"
#include "utils/Log.hpp"

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

        TextureResourceMetal::TextureResourceMetal(RenderDeviceMetal& initRenderDeviceMetal):
            renderDeviceMetal(initRenderDeviceMetal)
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

        bool TextureResourceMetal::init(const Size2& newSize,
                                        uint32_t newFlags,
                                        uint32_t newMipmaps,
                                        uint32_t newSampleCount,
                                        PixelFormat newPixelFormat)
        {
            if (!TextureResource::init(newSize,
                                       newFlags,
                                       newMipmaps,
                                       newSampleCount,
                                       newPixelFormat))
                return false;

            if (!createTexture()) return false;

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

            return updateSamplerState();
        }

        bool TextureResourceMetal::init(const std::vector<uint8_t>& newData,
                                        const Size2& newSize,
                                        uint32_t newFlags,
                                        uint32_t newMipmaps,
                                        PixelFormat newPixelFormat)
        {
            if (!TextureResource::init(newData,
                                       newSize,
                                       newFlags,
                                       newMipmaps,
                                       newPixelFormat))
                return false;

            if (!createTexture()) return false;

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

            return updateSamplerState();
        }

        bool TextureResourceMetal::init(const std::vector<Texture::Level>& newLevels,
                                        const Size2& newSize,
                                        uint32_t newFlags,
                                        PixelFormat newPixelFormat)
        {
            if (!TextureResource::init(newLevels,
                                       newSize,
                                       newFlags,
                                       newPixelFormat))
                return false;

            if (!createTexture())
                return false;

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

            return updateSamplerState();
        }

        bool TextureResourceMetal::setSize(const Size2& newSize)
        {
            if (!TextureResource::setSize(newSize))
                return false;

            if (!texture ||
                static_cast<NSUInteger>(size.width) != width ||
                static_cast<NSUInteger>(size.height) != height)
            {
                if (!createTexture()) return false;
            }

            return true;
        }

        bool TextureResourceMetal::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!TextureResource::setData(newData, newSize))
                return false;

            if (!texture ||
                static_cast<NSUInteger>(size.width) != width ||
                static_cast<NSUInteger>(size.height) != height)
            {
                if (!createTexture()) return false;
            }

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

            return true;
        }

        bool TextureResourceMetal::setFilter(Texture::Filter newFilter)
        {
            if (!TextureResource::setFilter(newFilter))
                return false;

            return updateSamplerState();
        }

        bool TextureResourceMetal::setAddressX(Texture::Address newAddressX)
        {
            if (!TextureResource::setAddressX(newAddressX))
                return false;

            return updateSamplerState();
        }

        bool TextureResourceMetal::setAddressY(Texture::Address newAddressY)
        {
            if (!TextureResource::setAddressY(newAddressY))
                return false;

            return updateSamplerState();
        }

        bool TextureResourceMetal::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            if (!TextureResource::setMaxAnisotropy(newMaxAnisotropy))
                return false;

            return updateSamplerState();
        }

        bool TextureResourceMetal::setClearColorBuffer(bool clear)
        {
            if (!TextureResource::setClearColorBuffer(clear))
                return false;

            colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;

            return true;
        }

        bool TextureResourceMetal::setClearDepthBuffer(bool clear)
        {
            if (!TextureResource::setClearDepthBuffer(clear))
                return false;

            depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;

            return true;
        }

        bool TextureResourceMetal::setClearColor(Color color)
        {
            if (!TextureResource::setClearColor(color))
                return false;

            if (!renderPassDescriptor)
            {
                Log(Log::Level::ERR) << "Render pass descriptor not initialized";
                return false;
            }

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                    clearColor.normG(),
                                                                                    clearColor.normB(),
                                                                                    clearColor.normA());

            return true;
        }

        bool TextureResourceMetal::setClearDepth(float clear)
        {
            if (!TextureResource::setClearDepth(clear))
                return false;

            if (!renderPassDescriptor)
            {
                Log(Log::Level::ERR) << "Render pass descriptor not initialized";
                return false;
            }

            renderPassDescriptor.depthAttachment.clearDepth = clearDepth;

            return true;
        }

        bool TextureResourceMetal::createTexture()
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

            width = static_cast<NSUInteger>(size.width);
            height = static_cast<NSUInteger>(size.height);

            if (width > 0 && height > 0)
            {
                MTLPixelFormat metalPixelFormat = getMetalPixelFormat(pixelFormat);

                if (metalPixelFormat == MTLPixelFormatInvalid)
                {
                    Log(Log::Level::ERR) << "Invalid pixel format";
                    return false;
                }

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
                {
                    Log(Log::Level::ERR) << "Failed to create Metal texture";
                    return false;
                }
            }

            if (flags & Texture::RENDER_TARGET)
            {
                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                if (!renderPassDescriptor)
                {
                    Log(Log::Level::ERR) << "Failed to create Metal render pass descriptor";
                    return false;
                }

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
                    {
                        Log(Log::Level::ERR) << "Failed to create MSAA texture";
                        return false;
                    }

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
                    {
                        Log(Log::Level::ERR) << "Failed to create depth texture";
                        return false;
                    }

                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                }
                else
                    renderPassDescriptor.depthAttachment.texture = nil;
            }

            return true;
        }

        bool TextureResourceMetal::updateSamplerState()
        {
            RenderDeviceMetal::SamplerStateDescriptor samplerDescriptor;
            samplerDescriptor.filter = (filter == Texture::Filter::DEFAULT) ? renderDeviceMetal.getTextureFilter() : filter;
            samplerDescriptor.addressX = addressX;
            samplerDescriptor.addressY = addressY;
            samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDeviceMetal.getMaxAnisotropy() : maxAnisotropy;

            if (samplerState) [samplerState release];
            samplerState = renderDeviceMetal.getSamplerState(samplerDescriptor);

            if (!samplerState)
            {
                Log(Log::Level::ERR) << "Failed to get Metal sampler state";
                return false;
            }

            [samplerState retain];

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
