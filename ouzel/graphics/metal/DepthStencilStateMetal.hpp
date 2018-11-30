// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef DEPTHSTENCILSTATEMETAL_HPP
#define DEPTHSTENCILSTATEMETAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#if defined(__OBJC__)
#  import <CoreVideo/CoreVideo.h>
#  import <QuartzCore/QuartzCore.h>
#  import <Metal/Metal.h>
typedef id<MTLDepthStencilState> MTLDepthStencilStatePtr;
#else
#  include <objc/objc.h>
typedef id MTLDepthStencilStatePtr;
#endif

#include "graphics/metal/RenderResourceMetal.hpp"
#include "graphics/DepthStencilState.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class DepthStencilStateMetal final: public RenderResourceMetal
        {
        public:
            DepthStencilStateMetal(RenderDeviceMetal& renderDeviceMetal,
                                   bool initDepthTest,
                                   bool initDepthWrite,
                                   DepthStencilState::CompareFunction initCompareFunction);
            ~DepthStencilStateMetal();

            MTLDepthStencilStatePtr getDepthStencilState() const { return depthStencilState; }

        private:
            MTLDepthStencilStatePtr depthStencilState = nil;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // DEPTHSTENCILSTATEMETAL_HPP
