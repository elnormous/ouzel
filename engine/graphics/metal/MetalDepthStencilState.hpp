#ifndef FLUXION_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
#define FLUXION_GRAPHICS_METALDEPTHSTENCILSTATE_HPP

#include "../../core/Setup.h"

#if FLUXION_COMPILE_METAL

#ifdef __OBJC__
#  import <CoreVideo/CoreVideo.h>
#  import <QuartzCore/QuartzCore.h>
#  import <Metal/Metal.h>
using MTLDepthStencilStatePtr = id<MTLDepthStencilState>;
#else
#  include <objc/objc.h>
using MTLDepthStencilStatePtr = id;
#endif

#include "MetalRenderResource.hpp"
#include "../CompareFunction.hpp"
#include "../StencilOperation.hpp"
#include "../../platform/objc/Pointer.hpp"

namespace fluxion::graphics::metal
{
    class RenderDevice;

    class DepthStencilState final: public RenderResource
    {
    public:
        DepthStencilState(RenderDevice& initRenderDevice,
                          bool initDepthTest,
                          bool initDepthWrite,
                          CompareFunction initCompareFunction,
                          bool initStencilEnabled,
                          std::uint32_t initStencilReadMask,
                          std::uint32_t initStencilWriteMask,
                          StencilOperation initFrontFaceStencilFailureOperation,
                          StencilOperation initFrontFaceStencilDepthFailureOperation,
                          StencilOperation initFrontFaceStencilPassOperation,
                          CompareFunction initFrontFaceStencilCompareFunction,
                          StencilOperation initBackFaceStencilFailureOperation,
                          StencilOperation initBackFaceStencilDepthFailureOperation,
                          StencilOperation initBackFaceStencilPassOperation,
                          CompareFunction initBackFaceStencilCompareFunction);

        auto& getDepthStencilState() const noexcept { return depthStencilState; }

    private:
        platform::objc::Pointer<MTLDepthStencilStatePtr> depthStencilState;
    };
}

#endif

#endif // FLUXION_GRAPHICS_METALDEPTHSTENCILSTATE_HPP
