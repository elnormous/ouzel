// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11DepthStencilState.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            constexpr D3D11_COMPARISON_FUNC getCompareFunction(CompareFunction compareFunction)
            {
                switch (compareFunction)
                {
                    case CompareFunction::NEVER: return D3D11_COMPARISON_NEVER;
                    case CompareFunction::LESS: return D3D11_COMPARISON_LESS;
                    case CompareFunction::EQUAL: return D3D11_COMPARISON_EQUAL;
                    case CompareFunction::LESS_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
                    case CompareFunction::GREATER: return D3D11_COMPARISON_GREATER;
                    case CompareFunction::NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
                    case CompareFunction::GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
                    case CompareFunction::ALWAYS: return D3D11_COMPARISON_ALWAYS;
                    default: return D3D11_COMPARISON_NEVER;
                }
            }

            constexpr D3D11_STENCIL_OP getStencilOperation(StencilOperation stencilOperation)
            {
                switch (stencilOperation)
                {
                    case StencilOperation::KEEP: return D3D11_STENCIL_OP_KEEP;
                    case StencilOperation::ZERO: return D3D11_STENCIL_OP_ZERO;
                    case StencilOperation::REPLACE: return D3D11_STENCIL_OP_REPLACE;
                    case StencilOperation::INCREMENT_CLAMP: return D3D11_STENCIL_OP_INCR_SAT;
                    case StencilOperation::DECREMENT_CLAMP: return D3D11_STENCIL_OP_DECR_SAT;
                    case StencilOperation::INVERT: return D3D11_STENCIL_OP_INVERT;
                    case StencilOperation::INCREMENT_WRAP: return D3D11_STENCIL_OP_INCR;
                    case StencilOperation::DECREMENT_WRAP: return D3D11_STENCIL_OP_DECR;
                    default: return D3D11_STENCIL_OP_KEEP;
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
                D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
                depthStencilStateDesc.DepthEnable = initDepthTest ? TRUE : FALSE;
                depthStencilStateDesc.DepthWriteMask = initDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
                depthStencilStateDesc.DepthFunc = getCompareFunction(initCompareFunction);
                depthStencilStateDesc.StencilEnable = initStencilEnabled ? TRUE : FALSE;
                depthStencilStateDesc.StencilReadMask = static_cast<UINT8>(initStencilReadMask);
                depthStencilStateDesc.StencilWriteMask = static_cast<UINT8>(initStencilWriteMask);
                depthStencilStateDesc.FrontFace.StencilFailOp = getStencilOperation(initFrontFaceStencilFailureOperation);
                depthStencilStateDesc.FrontFace.StencilDepthFailOp = getStencilOperation(initFrontFaceStencilDepthFailureOperation);
                depthStencilStateDesc.FrontFace.StencilPassOp = getStencilOperation(initFrontFaceStencilPassOperation);
                depthStencilStateDesc.FrontFace.StencilFunc = getCompareFunction(initFrontFaceStencilCompareFunction);
                depthStencilStateDesc.BackFace.StencilFailOp = getStencilOperation(initBackFaceStencilFailureOperation);
                depthStencilStateDesc.BackFace.StencilDepthFailOp = getStencilOperation(initBackFaceStencilDepthFailureOperation);
                depthStencilStateDesc.BackFace.StencilPassOp = getStencilOperation(initBackFaceStencilPassOperation);
                depthStencilStateDesc.BackFace.StencilFunc = getCompareFunction(initBackFaceStencilCompareFunction);

                HRESULT hr;
                if (FAILED(hr = renderDevice.getDevice()->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState)))
                    throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 depth stencil state");
            }

            DepthStencilState::~DepthStencilState()
            {
                if (depthStencilState) depthStencilState->Release();
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
