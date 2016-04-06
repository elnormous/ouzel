// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "BlendState.h"

#ifdef __OBJC__
#import <Metal/Metal.h>
typedef MTLRenderPipelineColorAttachmentDescriptor* MTLRenderPipelineColorAttachmentDescriptorPtr;
#else
#include <objc/objc.h>
typedef id MTLRenderPipelineColorAttachmentDescriptorPtr;
#endif

namespace ouzel
{
    namespace video
    {
        class MetalRenderer;

        class BlendStateMetal: public BlendState
        {
            friend MetalRenderer;
        public:
            virtual ~BlendStateMetal();

        protected:
            BlendStateMetal();

            void destroy();

            MTLRenderPipelineColorAttachmentDescriptorPtr _blendState = Nil;
        };
    } // namespace video
} // namespace ouzel
