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
            static D3D11_COMPARISON_FUNC getCompareFunction(ouzel::graphics::DepthStencilState::CompareFunction compareFunction)
            {
                switch (compareFunction)
                {
                    case ouzel::graphics::DepthStencilState::CompareFunction::NEVER: return D3D11_COMPARISON_NEVER;
                    case ouzel::graphics::DepthStencilState::CompareFunction::LESS: return D3D11_COMPARISON_LESS;
                    case ouzel::graphics::DepthStencilState::CompareFunction::EQUAL: return D3D11_COMPARISON_EQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::LESS_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::GREATER: return D3D11_COMPARISON_GREATER;
                    case ouzel::graphics::DepthStencilState::CompareFunction::NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
                    case ouzel::graphics::DepthStencilState::CompareFunction::ALWAYS: return D3D11_COMPARISON_ALWAYS;
                    default: return D3D11_COMPARISON_NEVER;
                }
            }

            static D3D11_STENCIL_OP getStencilOperation(ouzel::graphics::DepthStencilState::StencilOperation stencilOperation)
            {
                switch (stencilOperation)
                {
                    case ouzel::graphics::DepthStencilState::StencilOperation::KEEP: return D3D11_STENCIL_OP_KEEP;
                    case ouzel::graphics::DepthStencilState::StencilOperation::ZERO: return D3D11_STENCIL_OP_ZERO;
                    case ouzel::graphics::DepthStencilState::StencilOperation::REPLACE: return D3D11_STENCIL_OP_REPLACE;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INCREMENT_CLAMP: return D3D11_STENCIL_OP_INCR_SAT;
                    case ouzel::graphics::DepthStencilState::StencilOperation::DECREMENT_CLAMP: return D3D11_STENCIL_OP_DECR_SAT;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INVERT: return D3D11_STENCIL_OP_INVERT;
                    case ouzel::graphics::DepthStencilState::StencilOperation::INCREMENT_WRAP: return D3D11_STENCIL_OP_INCR;
                    case ouzel::graphics::DepthStencilState::StencilOperation::DECREMENT_WRAP: return D3D11_STENCIL_OP_DECR;
                    default: return D3D11_STENCIL_OP_KEEP;
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
