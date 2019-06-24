// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalDepthStencilState.hpp"
#include "MetalRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            static MTLCompareFunction getCompareFunction(CompareFunction compareFunction)
            {
                switch (compareFunction)
                {
                    case CompareFunction::NEVER: return MTLCompareFunctionNever;
                    case CompareFunction::LESS: return MTLCompareFunctionLess;
                    case CompareFunction::EQUAL: return MTLCompareFunctionEqual;
                    case CompareFunction::LESS_EQUAL: return MTLCompareFunctionLessEqual;
                    case CompareFunction::GREATER: return MTLCompareFunctionGreater;
                    case CompareFunction::NOT_EQUAL: return MTLCompareFunctionNotEqual;
                    case CompareFunction::GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
                    case CompareFunction::ALWAYS: return MTLCompareFunctionAlways;
                    default: return MTLCompareFunctionNever;
                }
            }

            static MTLStencilOperation getStencilOperation(StencilOperation stencilOperation)
            {
                switch (stencilOperation)
                {
                    case StencilOperation::KEEP: return MTLStencilOperationKeep;
                    case StencilOperation::ZERO: return MTLStencilOperationZero;
                    case StencilOperation::REPLACE: return MTLStencilOperationReplace;
                    case StencilOperation::INCREMENT_CLAMP: return MTLStencilOperationIncrementClamp;
                    case StencilOperation::DECREMENT_CLAMP: return MTLStencilOperationDecrementClamp;
                    case StencilOperation::INVERT: return MTLStencilOperationInvert;
                    case StencilOperation::INCREMENT_WRAP: return MTLStencilOperationIncrementWrap;
                    case StencilOperation::DECREMENT_WRAP: return MTLStencilOperationDecrementWrap;
                    default: return MTLStencilOperationKeep;
                }
            }

            DepthStencilState::DepthStencilState(RenderDevice& initRenderDevice,
                                                 bool initDepthTest,
                                                 bool initDepthWrite,
                                                 CompareFunction initCompareFunction,
                                                 bool initStencilEnabled,
                                                 uint32_t initStencilReadMask,
                                                 uint32_t initStencilWriteMask,
                                                 StencilOperation initFrontFaceStencilFailureOperation,
                                                 StencilOperation initFrontFaceStencilDepthFailureOperation,
                                                 StencilOperation initFrontFaceStencilPassOperation,
                                                 CompareFunction initFrontFaceStencilCompareFunction,
                                                 StencilOperation initBackFaceStencilFailureOperation,
                                                 StencilOperation initBackFaceStencilDepthFailureOperation,
                                                 StencilOperation initBackFaceStencilPassOperation,
                                                 CompareFunction initBackFaceStencilCompareFunction):
                RenderResource(initRenderDevice)
            {
                MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];

                depthStencilDescriptor.depthCompareFunction = initDepthTest ? getCompareFunction(initCompareFunction) : MTLCompareFunctionAlways; // depth read
                depthStencilDescriptor.depthWriteEnabled = initDepthWrite ? YES : NO; // depth write
                if (initStencilEnabled)
                {
                    depthStencilDescriptor.frontFaceStencil = [MTLStencilDescriptor new];
                    depthStencilDescriptor.frontFaceStencil.stencilFailureOperation = getStencilOperation(initFrontFaceStencilFailureOperation);
                    depthStencilDescriptor.frontFaceStencil.depthFailureOperation = getStencilOperation(initFrontFaceStencilDepthFailureOperation);
                    depthStencilDescriptor.frontFaceStencil.depthStencilPassOperation = getStencilOperation(initFrontFaceStencilPassOperation);
                    depthStencilDescriptor.frontFaceStencil.stencilCompareFunction = getCompareFunction(initFrontFaceStencilCompareFunction);
                    depthStencilDescriptor.frontFaceStencil.readMask = initStencilReadMask;
                    depthStencilDescriptor.frontFaceStencil.writeMask = initStencilWriteMask;

                    depthStencilDescriptor.backFaceStencil = [MTLStencilDescriptor new];
                    depthStencilDescriptor.backFaceStencil.stencilFailureOperation = getStencilOperation(initBackFaceStencilFailureOperation);
                    depthStencilDescriptor.backFaceStencil.depthFailureOperation = getStencilOperation(initBackFaceStencilDepthFailureOperation);
                    depthStencilDescriptor.backFaceStencil.depthStencilPassOperation = getStencilOperation(initBackFaceStencilPassOperation);
                    depthStencilDescriptor.backFaceStencil.stencilCompareFunction = getCompareFunction(initBackFaceStencilCompareFunction);
                    depthStencilDescriptor.backFaceStencil.readMask = initStencilReadMask;
                    depthStencilDescriptor.backFaceStencil.writeMask = initStencilWriteMask;
                }
                depthStencilState = [renderDevice.getDevice() newDepthStencilStateWithDescriptor:depthStencilDescriptor];
                [depthStencilDescriptor release];
            }

            DepthStencilState::~DepthStencilState()
            {
                if (depthStencilState) [depthStencilState release];
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
