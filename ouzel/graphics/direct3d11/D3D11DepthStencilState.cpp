// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11DepthStencilState.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        static D3D11_COMPARISON_FUNC getCompareFunction(DepthStencilState::CompareFunction compareFunction)
        {
            switch (compareFunction)
            {
                case DepthStencilState::CompareFunction::NEVER: return D3D11_COMPARISON_NEVER;
                case DepthStencilState::CompareFunction::LESS: return D3D11_COMPARISON_LESS;
                case DepthStencilState::CompareFunction::EQUAL: return D3D11_COMPARISON_EQUAL;
                case DepthStencilState::CompareFunction::LESS_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
                case DepthStencilState::CompareFunction::GREATER: return D3D11_COMPARISON_GREATER;
                case DepthStencilState::CompareFunction::NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
                case DepthStencilState::CompareFunction::GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
                case DepthStencilState::CompareFunction::ALWAYS: return D3D11_COMPARISON_ALWAYS;
                default: return D3D11_COMPARISON_NEVER;
            }

            return D3D11_COMPARISON_NEVER;
        }

        static D3D11_STENCIL_OP getStencilOperation(DepthStencilState::StencilOperation stencilOperation)
        {
            switch (stencilOperation)
            {
                case DepthStencilState::StencilOperation::KEEP: return D3D11_STENCIL_OP_KEEP;
                case DepthStencilState::StencilOperation::ZERO: return D3D11_STENCIL_OP_ZERO;
                case DepthStencilState::StencilOperation::REPLACE: return D3D11_STENCIL_OP_REPLACE;
                case DepthStencilState::StencilOperation::INCREMENT_CLAMP: return D3D11_STENCIL_OP_INCR_SAT;
                case DepthStencilState::StencilOperation::DECREMENT_CLAMP: return D3D11_STENCIL_OP_DECR_SAT;
                case DepthStencilState::StencilOperation::INVERT: return D3D11_STENCIL_OP_INVERT;
                case DepthStencilState::StencilOperation::INCREMENT_WRAP: return D3D11_STENCIL_OP_INCR;
                case DepthStencilState::StencilOperation::DECREMENT_WRAP: return D3D11_STENCIL_OP_DECR;
                default: return D3D11_STENCIL_OP_KEEP;
            }
        }

        D3D11DepthStencilState::D3D11DepthStencilState(D3D11RenderDevice& renderDeviceD3D11,
                                                       bool initDepthTest,
                                                       bool initDepthWrite,
                                                       DepthStencilState::CompareFunction initCompareFunction,
                                                       bool initStencilEnabled,
                                                       uint32_t initStencilReadMask,
                                                       uint32_t initStencilWriteMask,
                                                       const DepthStencilState::StencilDescriptor& initFrontFaceStencil,
                                                       const DepthStencilState::StencilDescriptor& initBackFaceStencil):
            D3D11RenderResource(renderDeviceD3D11)
        {
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
            depthStencilStateDesc.DepthEnable = initDepthTest ? TRUE : FALSE;
            depthStencilStateDesc.DepthWriteMask = initDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            depthStencilStateDesc.DepthFunc = getCompareFunction(initCompareFunction);
            depthStencilStateDesc.StencilEnable = initStencilEnabled ? TRUE : FALSE;
            depthStencilStateDesc.StencilReadMask = static_cast<UINT8>(initStencilReadMask);
            depthStencilStateDesc.StencilWriteMask = static_cast<UINT8>(initStencilWriteMask);
            depthStencilStateDesc.FrontFace.StencilFailOp = getStencilOperation(initFrontFaceStencil.failureOperation);
            depthStencilStateDesc.FrontFace.StencilDepthFailOp = getStencilOperation(initFrontFaceStencil.depthFailureOperation);
            depthStencilStateDesc.FrontFace.StencilPassOp = getStencilOperation(initFrontFaceStencil.passOperation);
            depthStencilStateDesc.FrontFace.StencilFunc = getCompareFunction(initFrontFaceStencil.compareFunction);
            depthStencilStateDesc.BackFace.StencilFailOp = getStencilOperation(initBackFaceStencil.failureOperation);
            depthStencilStateDesc.BackFace.StencilDepthFailOp = getStencilOperation(initBackFaceStencil.depthFailureOperation);
            depthStencilStateDesc.BackFace.StencilPassOp = getStencilOperation(initBackFaceStencil.passOperation);
            depthStencilStateDesc.BackFace.StencilFunc = getCompareFunction(initBackFaceStencil.compareFunction);

            HRESULT hr;
            if (FAILED(hr = renderDevice.getDevice()->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil state");
        }

        D3D11DepthStencilState::~D3D11DepthStencilState()
        {
            if (depthStencilState) depthStencilState->Release();
        }
    } // namespace graphics
} // namespace ouzel

#endif
