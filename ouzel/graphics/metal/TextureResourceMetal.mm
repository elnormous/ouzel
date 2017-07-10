// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "TextureResourceMetal.h"
#include "RendererMetal.h"
#include "core/Engine.h"
#include "math/MathUtils.h"
#include "utils/Log.h"

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

        TextureResourceMetal::TextureResourceMetal()
        {
        }

        TextureResourceMetal::~TextureResourceMetal()
        {
            if (msaaTexture)
            {
                [msaaTexture release];
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
            }

            if (texture)
            {
                [texture release];
            }

            if (samplerState)
            {
                [samplerState release];
            }
        }

        bool TextureResourceMetal::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                if ((dirty & DIRTY_DATA) ||
                    (dirty & DIRTY_SIZE))
                {
                    if (size.v[0] > 0 &&
                        size.v[1] > 0)
                    {
                        if (!texture || (dirty & DIRTY_SIZE))
                        {
                            if (texture)
                            {
                                [texture release];
                                texture = Nil;
                            }

                            if (msaaTexture)
                            {
                                [msaaTexture release];
                                msaaTexture = Nil;
                            }

                            if (renderPassDescriptor)
                            {
                                [renderPassDescriptor release];
                                renderPassDescriptor = Nil;
                            }

                            if (depthTexture)
                            {
                                [depthTexture release];
                                depthTexture = Nil;
                            }

                            width = static_cast<NSUInteger>(size.v[0]);
                            height = static_cast<NSUInteger>(size.v[1]);

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
                                textureDescriptor.usage = MTLTextureUsageShaderRead | (renderTarget ? MTLTextureUsageRenderTarget : 0);
                                colorFormat = textureDescriptor.pixelFormat;

                                texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

                                if (!texture)
                                {
                                    Log(Log::Level::ERR) << "Failed to create Metal texture";
                                    return false;
                                }
                            }

                            if (renderTarget)
                            {
                                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                                if (!renderPassDescriptor)
                                {
                                    Log(Log::Level::ERR) << "Failed to create Metal render pass descriptor";
                                    return false;
                                }

                                if (sampleCount > 1)
                                {
                                    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:static_cast<MTLPixelFormat>(colorFormat)
                                                                                                                    width:static_cast<NSUInteger>(size.v[0])
                                                                                                                   height:static_cast<NSUInteger>(size.v[1])
                                                                                                                mipmapped:NO];
                                    desc.textureType = MTLTextureType2DMultisample;
                                    desc.storageMode = MTLStorageModePrivate;
                                    desc.sampleCount = sampleCount;
                                    desc.usage = MTLTextureUsageRenderTarget;

                                    msaaTexture = [rendererMetal->getDevice() newTextureWithDescriptor: desc];

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

                                if (depth)
                                {
                                    depthFormat = MTLPixelFormatDepth32Float;

                                    MTLTextureDescriptor* desc = [MTLTextureDescriptor
                                                                  texture2DDescriptorWithPixelFormat:static_cast<MTLPixelFormat>(depthFormat)
                                                                  width:width height:height mipmapped:NO];

                                    desc.textureType = (sampleCount > 1) ? MTLTextureType2DMultisample : MTLTextureType2D;
                                    desc.storageMode = MTLStorageModePrivate;
                                    desc.sampleCount = sampleCount;
                                    desc.usage = MTLTextureUsageRenderTarget;

                                    depthTexture = [rendererMetal->getDevice() newTextureWithDescriptor:desc];

                                    if (!depthTexture)
                                    {
                                        Log(Log::Level::ERR) << "Failed to create depth texture";
                                        return false;
                                    }

                                    renderPassDescriptor.depthAttachment.texture = depthTexture;
                                }
                                else
                                {
                                    renderPassDescriptor.depthAttachment.texture = Nil;
                                }
                            }
                        }

                        if (!renderTarget)
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
                }

                if (dirty & DIRTY_PARAMETERS)
                {
                    colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
                    depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;

                    if (renderPassDescriptor)
                    {
                        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                                clearColor.normG(),
                                                                                                clearColor.normB(),
                                                                                                clearColor.normA());
                    }

                    if (samplerState) [samplerState release];

                    RendererMetal::SamplerStateDesc samplerDesc;
                    samplerDesc.filter = (filter == Texture::Filter::DEFAULT) ? rendererMetal->getTextureFilter() : filter;
                    samplerDesc.addressX = addressX;
                    samplerDesc.addressY = addressY;
                    samplerDesc.maxAnisotropy = (maxAnisotropy == 0) ? rendererMetal->getMaxAnisotropy() : maxAnisotropy;

                    samplerState = rendererMetal->getSamplerState(samplerDesc);

                    if (!samplerState)
                    {
                        Log(Log::Level::ERR) << "Failed to get Metal sampler state";
                        return false;
                    }

                    [samplerState retain];
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
