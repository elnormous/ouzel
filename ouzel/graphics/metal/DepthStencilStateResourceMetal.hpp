// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef DEPTHSTENCILSTATERESOURCEMETAL_HPP
#define DEPTHSTENCILSTATERESOURCEMETAL_HPP

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

        class DepthStencilStateResourceMetal final: public RenderResourceMetal
        {
        public:
            DepthStencilStateResourceMetal(RenderDeviceMetal& renderDeviceMetal,
                                           bool initDepthTest,
                                           bool initDepthWrite,
                                           DepthStencilState::CompareFunction initCompareFunction);
            ~DepthStencilStateResourceMetal();

            MTLDepthStencilStatePtr getDepthStencilState() const { return depthStencilState; }

        private:
            MTLDepthStencilStatePtr depthStencilState = nil;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // DEPTHSTENCILSTATERESOURCEMETAL_HPP
