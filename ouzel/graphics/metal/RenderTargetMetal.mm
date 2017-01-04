// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetMetal.h"
#include "TextureMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetMetal::RenderTargetMetal()
        {
        }

        RenderTargetMetal::~RenderTargetMetal()
        {
            if (msaaTexture)
            {
                [msaaTexture release];
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
            }
        }

        void RenderTargetMetal::free()
        {
            RenderTarget::free();

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
        }

        bool RenderTargetMetal::upload()
        {
            if (!RenderTarget::upload())
            {
                return false;
            }

            std::shared_ptr<TextureMetal> textureMetal = std::static_pointer_cast<TextureMetal>(texture);

            if (!textureMetal->upload())
            {
                return false;
            }

            if (!textureMetal->getTexture())
            {
                Log(Log::Level::ERR) << "Metal texture not initialized";
                return false;
            }

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
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(uploadData.clearColor.normR(),
                                                                                    uploadData.clearColor.normG(),
                                                                                    uploadData.clearColor.normB(),
                                                                                    uploadData.clearColor.normA());

            RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

            if (rendererMetal->getSampleCount() > 1)
            {
                if (!msaaTexture)
                {
                    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                                    width:static_cast<NSUInteger>(uploadData.size.v[0])
                                                                                                   height:static_cast<NSUInteger>(uploadData.size.v[1])
                                                                                                mipmapped:NO];
                    desc.textureType = MTLTextureType2DMultisample;
                    desc.storageMode = MTLStorageModePrivate;
                    desc.sampleCount = rendererMetal->getSampleCount();
                    desc.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [rendererMetal->getDevice() newTextureWithDescriptor: desc];

                    if (!msaaTexture)
                    {
                        Log(Log::Level::ERR) << "Failed to create MSAA texture";
                        return false;
                    }
                }

                renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                renderPassDescriptor.colorAttachments[0].texture = msaaTexture;
                renderPassDescriptor.colorAttachments[0].resolveTexture = textureMetal->getTexture();
            }
            else
            {
                renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                renderPassDescriptor.colorAttachments[0].texture = textureMetal->getTexture();
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
