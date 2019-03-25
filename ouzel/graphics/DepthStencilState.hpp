// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP

#include "graphics/GraphicsResource.hpp"
#include "utils/Inline.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class DepthStencilState final
        {
        public:
            enum class CompareFunction
            {
                NEVER,
                LESS,
                EQUAL,
                LESS_EQUAL,
                GREATER,
                NOT_EQUAL,
                GREATER_EQUAL,
                ALWAYS,
            };

            enum class StencilOperation
            {
                KEEP,
                ZERO,
                REPLACE,
                INCREMENT_CLAMP,
                DECREMENT_CLAMP,
                INVERT,
                INCREMENT_WRAP,
                DECREMENT_WRAP
            };

            struct StencilDescriptor
            {
                StencilOperation failureOperation = StencilOperation::KEEP;
                StencilOperation depthFailureOperation = StencilOperation::KEEP;
                StencilOperation passOperation = StencilOperation::KEEP;
                CompareFunction compareFunction = CompareFunction::ALWAYS;
            };

            DepthStencilState()
            {
            }

            explicit DepthStencilState(Renderer& initRenderer);
            DepthStencilState(Renderer& initRenderer,
                              bool initDepthTest,
                              bool initDepthWrite,
                              CompareFunction initCompareFunction,
                              bool initStencilEnabled,
                              uint32_t initStencilReadMask,
                              uint32_t initStencilWriteMask,
                              const StencilDescriptor& initFrontFaceStencil,
                              const StencilDescriptor& initBackFaceStencil);

            FORCEINLINE uintptr_t getResource() const { return resource.getId(); }

        private:
            Resource resource;

            bool depthTest = false;
            bool depthWrite = false;
            CompareFunction compareFunction;
            bool stencilEnabled = false;
            uint32_t stencilReadMask = 0xFFFFFFFF;
            uint32_t stencilWriteMask = 0xFFFFFFFF;
            StencilDescriptor frontFaceStencil;
            StencilDescriptor backFaceStencil;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP
