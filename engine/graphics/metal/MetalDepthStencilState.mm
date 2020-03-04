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
                Pointer<MTLDepthStencilDescriptor*> depthStencilDescriptor = [[MTLDepthStencilDescriptor alloc] init];

                depthStencilDescriptor.get().depthCompareFunction = initDepthTest ? getCompareFunction(initCompareFunction) : MTLCompareFunctionAlways; // depth read
                depthStencilDescriptor.get().depthWriteEnabled = initDepthWrite ? YES : NO; // depth write
                if (initStencilEnabled)
                {
                    Pointer<MTLStencilDescriptor*> frontFaceStencil = [[MTLStencilDescriptor alloc] init];
                    frontFaceStencil.get().stencilFailureOperation = getStencilOperation(initFrontFaceStencilFailureOperation);
                    frontFaceStencil.get().depthFailureOperation = getStencilOperation(initFrontFaceStencilDepthFailureOperation);
                    frontFaceStencil.get().depthStencilPassOperation = getStencilOperation(initFrontFaceStencilPassOperation);
                    frontFaceStencil.get().stencilCompareFunction = getCompareFunction(initFrontFaceStencilCompareFunction);
                    frontFaceStencil.get().readMask = initStencilReadMask;
                    frontFaceStencil.get().writeMask = initStencilWriteMask;

                    depthStencilDescriptor.get().frontFaceStencil = frontFaceStencil.get();

                    Pointer<MTLStencilDescriptor*> backFaceStencil = [[MTLStencilDescriptor alloc] init];
                    backFaceStencil.get().stencilFailureOperation = getStencilOperation(initBackFaceStencilFailureOperation);
                    backFaceStencil.get().depthFailureOperation = getStencilOperation(initBackFaceStencilDepthFailureOperation);
                    backFaceStencil.get().depthStencilPassOperation = getStencilOperation(initBackFaceStencilPassOperation);
                    backFaceStencil.get().stencilCompareFunction = getCompareFunction(initBackFaceStencilCompareFunction);
                    backFaceStencil.get().readMask = initStencilReadMask;
                    backFaceStencil.get().writeMask = initStencilWriteMask;

                    depthStencilDescriptor.get().backFaceStencil = backFaceStencil.get();
                }
                depthStencilState = [renderDevice.getDevice().get() newDepthStencilStateWithDescriptor:depthStencilDescriptor.get()];
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
