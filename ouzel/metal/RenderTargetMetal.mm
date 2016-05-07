// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetMetal.h"
#include "TextureMetal.h"
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
            destroy();
        }

        void RenderTargetMetal::destroy()
        {
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

            std::shared_ptr<TextureMetal> textureMetal(new TextureMetal());

            if (!textureMetal->init(size, false, false))
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
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());
            renderPassDescriptor.colorAttachments[0].texture = textureMetal->getTexture();

            return true;
        }
    } // namespace graphics
} // namespace ouzel
