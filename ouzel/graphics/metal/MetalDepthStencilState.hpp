// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP

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

#include "graphics/metal/MetalRenderResource.hpp"
#include "graphics/DepthStencilState.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            class RenderDevice;

            class DepthStencilState final: public RenderResource
            {
            public:
                DepthStencilState(RenderDevice& renderDevice,
                                  bool initDepthTest,
                                  bool initDepthWrite,
                                  ouzel::graphics::DepthStencilState::CompareFunction initCompareFunction,
                                  bool initStencilEnabled,
                                  uint32_t initStencilReadMask,
                                  uint32_t initStencilWriteMask,
                                  const ouzel::graphics::DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                  const ouzel::graphics::DepthStencilState::StencilDescriptor& initBackFaceStencil);
                ~DepthStencilState();

                inline MTLDepthStencilStatePtr getDepthStencilState() const { return depthStencilState; }

            private:
                MTLDepthStencilStatePtr depthStencilState = nil;
            };
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
