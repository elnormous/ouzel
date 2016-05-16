// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "BlendState.h"

#if defined(__OBJC__)
#import <Metal/Metal.h>
#else
#include <objc/objc.h>
typedef NSUInteger MTLBlendFactor;
typedef NSUInteger MTLBlendOperation;
#endif

namespace ouzel
{
    namespace graphics
    {
        class MetalRenderer;

        class BlendStateMetal: public BlendState
        {
            friend MetalRenderer;
        public:
            virtual ~BlendStateMetal();

            static MTLBlendFactor getBlendFactor(BlendFactor blendFactor);
            static MTLBlendOperation getBlendOperation(BlendOperation blendOperation);

        protected:
            BlendStateMetal();
        };
    } // namespace graphics
} // namespace ouzel
