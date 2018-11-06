// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "DepthStencilStateResourceD3D11.hpp"
#include "RenderDeviceD3D11.hpp"
#include "utils/Errors.hpp"

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
            }

            return D3D11_COMPARISON_NEVER;
        }

        DepthStencilStateResourceD3D11::DepthStencilStateResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                                                                       bool initDepthTest,
                                                                       bool initDepthWrite,
                                                                       DepthStencilState::CompareFunction initCompareFunction):
            RenderResourceD3D11(renderDeviceD3D11)
        {
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
            depthStencilStateDesc.DepthEnable = initDepthTest ? TRUE : FALSE;
            depthStencilStateDesc.DepthWriteMask = initDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            depthStencilStateDesc.DepthFunc = getCompareFunction(initCompareFunction);
            depthStencilStateDesc.StencilEnable = FALSE;
            depthStencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            depthStencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
            depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

            HRESULT hr = renderDevice.getDevice()->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
            if (FAILED(hr))
                throw SystemError("Failed to create Direct3D 11 depth stencil state, error: " + std::to_string(hr));
        }

        DepthStencilStateResourceD3D11::~DepthStencilStateResourceD3D11()
        {
            if (depthStencilState) depthStencilState->Release();
        }
    } // namespace graphics
} // namespace ouzel

#endif
