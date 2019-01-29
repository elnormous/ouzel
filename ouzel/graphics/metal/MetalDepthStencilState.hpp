// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
        class MetalRenderDevice;

        class MetalDepthStencilState final: public MetalRenderResource
        {
        public:
            MetalDepthStencilState(MetalRenderDevice& renderDeviceMetal,
                                   bool initDepthTest,
                                   bool initDepthWrite,
                                   DepthStencilState::CompareFunction initCompareFunction,
                                   bool initStencilEnabled,
                                   uint32_t initStencilReadMask,
                                   uint32_t initStencilWriteMask,
                                   const DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                   const DepthStencilState::StencilDescriptor& initBackFaceStencil);
            ~MetalDepthStencilState();

            MTLDepthStencilStatePtr getDepthStencilState() const { return depthStencilState; }

        private:
            MTLDepthStencilStatePtr depthStencilState = nil;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
