// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11BlendState.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            namespace
            {
                constexpr D3D11_BLEND getBlendFactor(BlendFactor blendFactor)
                {
                    switch (blendFactor)
                    {
                        case BlendFactor::Zero: return D3D11_BLEND_ZERO;
                        case BlendFactor::One: return D3D11_BLEND_ONE;
                        case BlendFactor::SrcColor: return D3D11_BLEND_SRC_COLOR;
                        case BlendFactor::InvSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
                        case BlendFactor::SrcAlpha: return D3D11_BLEND_SRC_ALPHA;
                        case BlendFactor::InvSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
                        case BlendFactor::DestAlpha: return D3D11_BLEND_DEST_ALPHA;
                        case BlendFactor::InvDestAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
                        case BlendFactor::DestColor: return D3D11_BLEND_DEST_COLOR;
                        case BlendFactor::InvDestColor: return D3D11_BLEND_INV_DEST_COLOR;
                        case BlendFactor::SrcAlphaSat: return D3D11_BLEND_SRC_ALPHA_SAT;
                        case BlendFactor::BlendFactor: return D3D11_BLEND_BLEND_FACTOR;
                        case BlendFactor::InvBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
                        default: return D3D11_BLEND_ZERO;
                    }
                }

                constexpr D3D11_BLEND_OP getBlendOperation(BlendOperation blendOperation)
                {
                    switch (blendOperation)
                    {
                        case BlendOperation::Add: return D3D11_BLEND_OP_ADD;
                        case BlendOperation::Subtract: return D3D11_BLEND_OP_SUBTRACT;
                        case BlendOperation::RevSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
                        case BlendOperation::Min: return D3D11_BLEND_OP_MIN;
                        case BlendOperation::Max: return D3D11_BLEND_OP_MAX;
                        default: return D3D11_BLEND_OP_ADD;
                    }
                }
            }

            BlendState::BlendState(RenderDevice& initRenderDevice,
                                   bool enableBlending,
                                   BlendFactor colorBlendSource,
                                   BlendFactor colorBlendDest,
                                   BlendOperation colorOperation,
                                   BlendFactor alphaBlendSource,
                                   BlendFactor alphaBlendDest,
                                   BlendOperation alphaOperation,
                                   uint8_t colorMask):
                RenderResource(initRenderDevice)
            {
                D3D11_BLEND_DESC blendStateDesc;
                blendStateDesc.AlphaToCoverageEnable = FALSE;
                blendStateDesc.IndependentBlendEnable = FALSE;

                D3D11_RENDER_TARGET_BLEND_DESC targetBlendDesc;
                targetBlendDesc.BlendEnable = enableBlending ? TRUE : FALSE;
                targetBlendDesc.SrcBlend = getBlendFactor(colorBlendSource);
                targetBlendDesc.DestBlend = getBlendFactor(colorBlendDest);
                targetBlendDesc.BlendOp = getBlendOperation(colorOperation);
                targetBlendDesc.SrcBlendAlpha = getBlendFactor(alphaBlendSource);
                targetBlendDesc.DestBlendAlpha = getBlendFactor(alphaBlendDest);
                targetBlendDesc.BlendOpAlpha = getBlendOperation(alphaOperation);
                targetBlendDesc.RenderTargetWriteMask = 0;
                if (colorMask & ColorMask::Red) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
                if (colorMask & ColorMask::Green) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
                if (colorMask & ColorMask::Blue) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
                if (colorMask & ColorMask::Alpha) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
                blendStateDesc.RenderTarget[0] = targetBlendDesc;

                if (blendState) blendState->Release();

				ID3D11BlendState* newBlendState;

                HRESULT hr;
                if (FAILED(hr = renderDevice.getDevice()->CreateBlendState(&blendStateDesc, &newBlendState)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to create Direct3D 11 blend state");

				blendState = newBlendState;
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
