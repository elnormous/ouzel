// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
                default: return MTLCompareFunctionNever;
            }
        }

        static MTLStencilOperation getStencilOperation(DepthStencilState::StencilOperation stencilOperation)
        {
            switch (stencilOperation)
            {
                case DepthStencilState::StencilOperation::KEEP: return MTLStencilOperationKeep;
                case DepthStencilState::StencilOperation::ZERO: return MTLStencilOperationZero;
                case DepthStencilState::StencilOperation::REPLACE: return MTLStencilOperationReplace;
                case DepthStencilState::StencilOperation::INCREMENT_CLAMP: return MTLStencilOperationIncrementClamp;
                case DepthStencilState::StencilOperation::DECREMENT_CLAMP: return MTLStencilOperationDecrementClamp;
                case DepthStencilState::StencilOperation::INVERT: return MTLStencilOperationInvert;
                case DepthStencilState::StencilOperation::INCREMENT_WRAP: return MTLStencilOperationIncrementWrap;
                case DepthStencilState::StencilOperation::DECREMENT_WRAP: return MTLStencilOperationDecrementWrap;
                default: return MTLStencilOperationKeep;
            }
        }

        MetalDepthStencilState::MetalDepthStencilState(MetalRenderDevice& renderDeviceMetal,
                                                       bool initDepthTest,
                                                       bool initDepthWrite,
                                                       DepthStencilState::CompareFunction initCompareFunction,
                                                       bool initStencilEnabled,
                                                       uint32_t initStencilReadMask,
                                                       uint32_t initStencilWriteMask,
                                                       const DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                                       const DepthStencilState::StencilDescriptor& initBackFaceStencil):
            MetalRenderResource(renderDeviceMetal)
        {
            MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];

            depthStencilDescriptor.depthCompareFunction = initDepthTest ? getCompareFunction(initCompareFunction) : MTLCompareFunctionAlways; // depth read
            depthStencilDescriptor.depthWriteEnabled = initDepthWrite ? YES : NO; // depth write
            if (initStencilEnabled)
            {
                depthStencilDescriptor.frontFaceStencil = [MTLStencilDescriptor new];
                depthStencilDescriptor.frontFaceStencil.stencilFailureOperation = getStencilOperation(initFrontFaceStencil.failureOperation);
                depthStencilDescriptor.frontFaceStencil.depthFailureOperation = getStencilOperation(initFrontFaceStencil.depthFailureOperation);
                depthStencilDescriptor.frontFaceStencil.depthStencilPassOperation = getStencilOperation(initFrontFaceStencil.passOperation);
                depthStencilDescriptor.frontFaceStencil.stencilCompareFunction = getCompareFunction(initFrontFaceStencil.compareFunction);
                depthStencilDescriptor.frontFaceStencil.readMask = initStencilReadMask;
                depthStencilDescriptor.frontFaceStencil.writeMask = initStencilWriteMask;

                depthStencilDescriptor.backFaceStencil = [MTLStencilDescriptor new];
                depthStencilDescriptor.backFaceStencil.stencilFailureOperation = getStencilOperation(initBackFaceStencil.failureOperation);
                depthStencilDescriptor.backFaceStencil.depthFailureOperation = getStencilOperation(initBackFaceStencil.depthFailureOperation);
                depthStencilDescriptor.backFaceStencil.depthStencilPassOperation = getStencilOperation(initBackFaceStencil.passOperation);
                depthStencilDescriptor.backFaceStencil.stencilCompareFunction = getCompareFunction(initBackFaceStencil.compareFunction);
                depthStencilDescriptor.backFaceStencil.readMask = initStencilReadMask;
                depthStencilDescriptor.backFaceStencil.writeMask = initStencilWriteMask;
            }
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
