// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP
#define OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP

#include <cstdint>

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
                INCREMENT,
                DECREMENT
            };

            struct StencilDescriptor
            {
                StencilOperation failureOperation = StencilOperation::KEEP;
                StencilOperation depthFailureOperation = StencilOperation::KEEP;
                StencilOperation passOperation = StencilOperation::KEEP;
                CompareFunction compareFunction = CompareFunction::ALWAYS;
            };

            explicit DepthStencilState(Renderer& initRenderer);
            DepthStencilState(Renderer& initRenderer,
                              bool initDepthTest,
                              bool initDepthWrite,
                              CompareFunction initCompareFunction,
                              bool initStencilEnabled,
                              uint32_t initStencilReadMask,
                              uint32_t initStencilWriteMask,
                              StencilDescriptor initFrontFaceStencil,
                              StencilDescriptor initBackFaceStencil);
            ~DepthStencilState();

            DepthStencilState(const DepthStencilState&) = delete;
            DepthStencilState& operator=(const DepthStencilState&) = delete;

            DepthStencilState(DepthStencilState&&) = delete;
            DepthStencilState& operator=(DepthStencilState&&) = delete;

            void init(bool newDepthTest,
                      bool newDepthWrite,
                      CompareFunction newCompareFunction,
                      bool initStencilEnabled,
                      uint32_t newStencilReadMask,
                      uint32_t newStencilWriteMask,
                      StencilDescriptor initFrontFaceStencil,
                      StencilDescriptor initBackFaceStencil);

            inline uintptr_t getResource() const { return resource; }

        private:
            Renderer& renderer;
            uintptr_t resource = 0;

            bool depthTest = false;
            bool depthWrite = false;
            CompareFunction compareFunction;
            bool stencilEnabled = false;
            uint32_t stencilReadMask;
            uint32_t stencilWriteMask;
            StencilDescriptor frontFaceStencil;
            StencilDescriptor backFaceStencil;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_DEPTHSTENCILSTATE_HPP
