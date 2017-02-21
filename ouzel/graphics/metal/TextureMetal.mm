// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "math/MathUtils.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        TextureMetal::TextureMetal()
        {
        }

        TextureMetal::~TextureMetal()
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
        }

        bool TextureMetal::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                if (size.v[0] > 0 &&
                    size.v[1] > 0)
                {
                    if (!texture ||
                        static_cast<NSUInteger>(size.v[0]) != width ||
                        static_cast<NSUInteger>(size.v[1]) != height)
                    {
                        if (texture) [texture release];

                        width = static_cast<NSUInteger>(size.v[0]);
                        height = static_cast<NSUInteger>(size.v[1]);

                        if (width > 0 && height > 0)
                        {
                            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                                         width:width
                                                                                                                        height:height
                                                                                                                     mipmapped:mipMapsGenerated ? YES : NO];
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
                            colorBufferLoadAction = clearColorBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;
                            depthBufferLoadAction = clearDepthBuffer ? MTLLoadActionClear : MTLLoadActionDontCare;

                            if (!renderPassDescriptor)
                            {
                                renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                                if (!renderPassDescriptor)
                                {
                                    Log(Log::Level::ERR) << "Failed to create Metal render pass descriptor";
                                    return false;
                                }
                            }

                            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.normR(),
                                                                                                    clearColor.normG(),
                                                                                                    clearColor.normB(),
                                                                                                    clearColor.normA());

                            if (sampleCount > 1)
                            {
                                if (msaaTexture) [msaaTexture release];

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
                                if (depthTexture) [depthTexture release];

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

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
