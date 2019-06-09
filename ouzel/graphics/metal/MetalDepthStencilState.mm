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
            static MTLCompareFunction getCompareFunction(ouzel::graphics::DepthStencilState::CompareFunction compareFunction)
            {
                switch (compareFunction)
                {
                    case ouzel::graphics::DepthStencilState::CompareFunction::NEVER: return MTLCompareFunctionNever;
                    case ouzel::graphics::DepthStencilState::CompareFunction::LESS: return MTLCompareFunctionLess;
                    case ouzel::graphics::DepthStencilState::CompareFunction::EQUAL: return MTLCompareFunctionEqual;
                    case ouzel::graphics::DepthStencilState::CompareFunction::LESS_EQUAL: return MTLCompareFunctionLessEqual;
                    case ouzel::graphics::DepthStencilState::CompareFunction::GREATER: return MTLCompareFunctionGreater;
                    case ouzel::graphics::DepthStencilState::CompareFunction::NOT_EQUAL: return MTLCompareFunctionNotEqual;
                    case ouzel::graphics::DepthStencilState::CompareFunction::GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
                    case ouzel::graphics::DepthStencilState::CompareFunction::ALWAYS: return MTLCompareFunctionAlways;
                    default: return MTLCompareFunctionNever;
                }
            }

            static MTLStencilOperation getStencilOperation(ouzel::graphics::DepthStencilState::StencilOperation stencilOperation)
            {
                switch (stencilOperation)
                {
                    case ouzel::graphics::DepthStencilState::StencilOperation::KEEP: return MTLStencilOperationKeep;
                    case ouzel::graphics::DepthStencilState::StencilOperation::ZERO: return MTLStencilOperationZero;
                    case ouzel::graphics::DepthStencilState::StencilOperation::REPLACE: return MTLStencilOperationReplace;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INCREMENT_CLAMP: return MTLStencilOperationIncrementClamp;
                    case ouzel::graphics::DepthStencilState::StencilOperation::DECREMENT_CLAMP: return MTLStencilOperationDecrementClamp;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INVERT: return MTLStencilOperationInvert;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INCREMENT_WRAP: return MTLStencilOperationIncrementWrap;
                    case ouzel::graphics::DepthStencilState::StencilOperation::DECREMENT_WRAP: return MTLStencilOperationDecrementWrap;
                    default: return MTLStencilOperationKeep;
                }
            }

            DepthStencilState::DepthStencilState(RenderDevice& renderDevice,
                                                 bool initDepthTest,
                                                 bool initDepthWrite,
                                                 ouzel::graphics::DepthStencilState::CompareFunction initCompareFunction,
                                                 bool initStencilEnabled,
                                                 uint32_t initStencilReadMask,
                                                 uint32_t initStencilWriteMask,
                                                 const ouzel::graphics::DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                                 const ouzel::graphics::DepthStencilState::StencilDescriptor& initBackFaceStencil):
                RenderResource(renderDevice)
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

            DepthStencilState::~DepthStencilState()
            {
                if (depthStencilState) [depthStencilState release];
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
