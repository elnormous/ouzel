// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP

#include "RenderDevice.hpp"
#include "CompareFunction.hpp"
#include "StencilOperation.hpp"

namespace ouzel::graphics
{
    class Graphics;

    class DepthStencilState final
    {
    public:
        DepthStencilState() = default;

        explicit DepthStencilState(Graphics& initGraphics);
        DepthStencilState(Graphics& initGraphics,
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

        auto& getResource() const noexcept { return resource; }

    private:
        RenderDevice::Resource resource;

        bool depthTest = false;
        bool depthWrite = false;
        CompareFunction compareFunction;
        bool stencilEnabled = false;
        std::uint32_t stencilReadMask = 0xFFFFFFFF;
        std::uint32_t stencilWriteMask = 0xFFFFFFFF;
        StencilOperation frontFaceStencilFailureOperation;
        StencilOperation frontFaceStencilDepthFailureOperation;
        StencilOperation frontFaceStencilPassOperation;
        CompareFunction frontFaceStencilCompareFunction;
        StencilOperation backFaceStencilFailureOperation;
        StencilOperation backFaceStencilDepthFailureOperation;
        StencilOperation backFaceStencilPassOperation;
        CompareFunction backFaceStencilCompareFunction;
    };
}

#endif // OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP
