// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetMetal.h"
#include "TextureMetal.h"
#include "Engine.h"
#include "RendererMetal.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetMetal::RenderTargetMetal()
        {

        }

        RenderTargetMetal::~RenderTargetMetal()
        {
            free();
        }

        void RenderTargetMetal::free()
        {
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
            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            free();

            std::shared_ptr<TextureMetal> textureMetal(new TextureMetal());

            if (!textureMetal->init(size, false, false, true))
            {
                return false;
            }

            texture = textureMetal;

            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
            {
                log("Failed to create Metal render pass descriptor");
                return false;
            }

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            if (rendererMetal->getSampleCount() > 1)
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

        void RenderTargetMetal::setClearColor(Color color)
        {
            RenderTarget::setClearColor(color);

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());
        }
    } // namespace graphics
} // namespace ouzel
