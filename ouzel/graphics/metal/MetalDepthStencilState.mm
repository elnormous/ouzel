// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalDepthStencilState.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        static MTLCompareFunction getCompareFunction(DepthStencilState::CompareFunction compareFunction)
        {
            switch (compareFunction)
            {
                case DepthStencilState::CompareFunction::NEVER: return MTLCompareFunctionNever;
                case DepthStencilState::CompareFunction::LESS: return MTLCompareFunctionLess;
                case DepthStencilState::CompareFunction::EQUAL: return MTLCompareFunctionEqual;
                case DepthStencilState::CompareFunction::LESS_EQUAL: return MTLCompareFunctionLessEqual;
                case DepthStencilState::CompareFunction::GREATER: return MTLCompareFunctionGreater;
                case DepthStencilState::CompareFunction::NOT_EQUAL: return MTLCompareFunctionNotEqual;
                case DepthStencilState::CompareFunction::GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
                case DepthStencilState::CompareFunction::ALWAYS: return MTLCompareFunctionAlways;
            }

            return MTLCompareFunctionNever;
        }

        MetalDepthStencilState::MetalDepthStencilState(MetalRenderDevice& renderDeviceMetal,
                                                       bool initDepthTest,
                                                       bool initDepthWrite,
                                                       DepthStencilState::CompareFunction initCompareFunction):
            MetalRenderResource(renderDeviceMetal)
        {
            MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];

            depthStencilDescriptor.depthCompareFunction = initDepthTest ? getCompareFunction(initCompareFunction) : MTLCompareFunctionAlways; // depth read
            depthStencilDescriptor.depthWriteEnabled = initDepthWrite ? YES : NO; // depth write
            depthStencilState = [renderDevice.getDevice() newDepthStencilStateWithDescriptor:depthStencilDescriptor];
            [depthStencilDescriptor release];
        }

        MetalDepthStencilState::~MetalDepthStencilState()
        {
            if (depthStencilState) [depthStencilState release];
        }
    } // namespace graphics
} // namespace ouzel

#endif
