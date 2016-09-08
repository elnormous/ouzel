// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetMetal.h"
#include "TextureMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetMetal::RenderTargetMetal():
            dirty(false)
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
            std::lock_guard<std::mutex> lock(dataMutex);

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

        bool RenderTargetMetal::init(const Size2& newSize, bool depthBuffer)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            std::shared_ptr<TextureMetal> textureMetal(new TextureMetal());

            if (!textureMetal->init(size, false, false, true))
            {
                return false;
            }

            texture = textureMetal;

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        void RenderTargetMetal::setClearColor(Color color)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            RenderTarget::setClearColor(color);

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());
        }

        bool RenderTargetMetal::upload()
        {
            if (dirty)
            {
                if (!texture->upload())
                {
                    return false;
                }

                std::lock_guard<std::mutex> lock(dataMutex);

                std::shared_ptr<TextureMetal> textureMetal = std::static_pointer_cast<TextureMetal>(texture);

                if (!textureMetal->getTexture())
                {
                    log(LOG_LEVEL_ERROR, "Metal texture not initialized");
                    return false;
                }

                if (!renderPassDescriptor)
                {
                    renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

                    if (!renderPassDescriptor)
                    {
                        log(LOG_LEVEL_ERROR, "Failed to create Metal render pass descriptor");
                        return false;
                    }
                }

                renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());

                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (rendererMetal->getSampleCount() > 1)
                {
                    if (!msaaTexture)
                    {
                        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                                        width:static_cast<NSUInteger>(size.width)
                                                                                                       height:static_cast<NSUInteger>(size.height)
                                                                                                    mipmapped:NO];
                        desc.textureType = MTLTextureType2DMultisample;
                        desc.storageMode = MTLStorageModePrivate;
                        desc.sampleCount = rendererMetal->getSampleCount();
                        desc.usage = MTLTextureUsageRenderTarget;

                        msaaTexture = [rendererMetal->getDevice() newTextureWithDescriptor: desc];

                        if (!msaaTexture)
                        {
                            log(LOG_LEVEL_ERROR, "Failed to create MSAA texture");
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

                ready = true;
                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
