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
            namespace
            {
                constexpr MTLCompareFunction getCompareFunction(CompareFunction compareFunction)
                {
                    switch (compareFunction)
                    {
                        case CompareFunction::NeverPass: return MTLCompareFunctionNever;
                        case CompareFunction::PassIfLess: return MTLCompareFunctionLess;
                        case CompareFunction::PassIfEqual: return MTLCompareFunctionEqual;
                        case CompareFunction::PassIfLessEqual: return MTLCompareFunctionLessEqual;
                        case CompareFunction::PassIfGreater: return MTLCompareFunctionGreater;
                        case CompareFunction::PassIfNotEqual: return MTLCompareFunctionNotEqual;
                        case CompareFunction::PassIfGreaterEqual: return MTLCompareFunctionGreaterEqual;
                        case CompareFunction::AlwaysPass: return MTLCompareFunctionAlways;
                        default: return MTLCompareFunctionNever;
                    }
                }

                constexpr MTLStencilOperation getStencilOperation(StencilOperation stencilOperation)
                {
                    switch (stencilOperation)
                    {
                        case StencilOperation::Keep: return MTLStencilOperationKeep;
                        case StencilOperation::Zero: return MTLStencilOperationZero;
                        case StencilOperation::Replace: return MTLStencilOperationReplace;
                        case StencilOperation::IncrementClamp: return MTLStencilOperationIncrementClamp;
                        case StencilOperation::DecrementClamp: return MTLStencilOperationDecrementClamp;
                        case StencilOperation::Invert: return MTLStencilOperationInvert;
                        case StencilOperation::IncrementWrap: return MTLStencilOperationIncrementWrap;
                        case StencilOperation::DecrementWrap: return MTLStencilOperationDecrementWrap;
                        default: return MTLStencilOperationKeep;
                    }
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
                    MTLStencilDescriptor* frontFaceStencil = [MTLStencilDescriptor new];
                    frontFaceStencil.stencilFailureOperation = getStencilOperation(initFrontFaceStencilFailureOperation);
                    frontFaceStencil.depthFailureOperation = getStencilOperation(initFrontFaceStencilDepthFailureOperation);
                    frontFaceStencil.depthStencilPassOperation = getStencilOperation(initFrontFaceStencilPassOperation);
                    frontFaceStencil.stencilCompareFunction = getCompareFunction(initFrontFaceStencilCompareFunction);
                    frontFaceStencil.readMask = initStencilReadMask;
                    frontFaceStencil.writeMask = initStencilWriteMask;

                    MTLStencilDescriptor* backFaceStencil = [MTLStencilDescriptor new];
                    backFaceStencil.stencilFailureOperation = getStencilOperation(initBackFaceStencilFailureOperation);
                    backFaceStencil.depthFailureOperation = getStencilOperation(initBackFaceStencilDepthFailureOperation);
                    backFaceStencil.depthStencilPassOperation = getStencilOperation(initBackFaceStencilPassOperation);
                    backFaceStencil.stencilCompareFunction = getCompareFunction(initBackFaceStencilCompareFunction);
                    backFaceStencil.readMask = initStencilReadMask;
                    backFaceStencil.writeMask = initStencilWriteMask;

                    depthStencilDescriptor.frontFaceStencil = frontFaceStencil;
                    depthStencilDescriptor.backFaceStencil = backFaceStencil;

                    [frontFaceStencil release];
                    [backFaceStencil release];
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
