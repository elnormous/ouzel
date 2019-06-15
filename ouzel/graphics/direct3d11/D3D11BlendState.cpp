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
            static D3D11_BLEND getBlendFactor(BlendFactor blendFactor)
            {
                switch (blendFactor)
                {
                    case BlendFactor::ZERO: return D3D11_BLEND_ZERO;
                    case BlendFactor::ONE: return D3D11_BLEND_ONE;
                    case BlendFactor::SRC_COLOR: return D3D11_BLEND_SRC_COLOR;
                    case BlendFactor::INV_SRC_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
                    case BlendFactor::SRC_ALPHA: return D3D11_BLEND_SRC_ALPHA;
                    case BlendFactor::INV_SRC_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
                    case BlendFactor::DEST_ALPHA: return D3D11_BLEND_DEST_ALPHA;
                    case BlendFactor::INV_DEST_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
                    case BlendFactor::DEST_COLOR: return D3D11_BLEND_DEST_COLOR;
                    case BlendFactor::INV_DEST_COLOR: return D3D11_BLEND_INV_DEST_COLOR;
                    case BlendFactor::SRC_ALPHA_SAT: return D3D11_BLEND_SRC_ALPHA_SAT;
                    case BlendFactor::BLEND_FACTOR: return D3D11_BLEND_BLEND_FACTOR;
                    case BlendFactor::INV_BLEND_FACTOR: return D3D11_BLEND_INV_BLEND_FACTOR;
                    default: return D3D11_BLEND_ZERO;
                }
            }

            static D3D11_BLEND_OP getBlendOperation(BlendOperation blendOperation)
            {
                switch (blendOperation)
                {
                    case BlendOperation::ADD: return D3D11_BLEND_OP_ADD;
                    case BlendOperation::SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
                    case BlendOperation::REV_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
                    case BlendOperation::MIN: return D3D11_BLEND_OP_MIN;
                    case BlendOperation::MAX: return D3D11_BLEND_OP_MAX;
                    default: return D3D11_BLEND_OP_ADD;
                }
            }

            BlendState::BlendState(RenderDevice& renderDevice,
                                   bool enableBlending,
                                   BlendFactor colorBlendSource,
                                   BlendFactor colorBlendDest,
                                   BlendOperation colorOperation,
                                   BlendFactor alphaBlendSource,
                                   BlendFactor alphaBlendDest,
                                   BlendOperation alphaOperation,
                                   uint8_t colorMask):
                RenderResource(renderDevice)
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
                if (colorMask & graphics::ColorMask::COLOR_MASK_RED) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
                if (colorMask & graphics::ColorMask::COLOR_MASK_GREEN) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
                if (colorMask & graphics::ColorMask::COLOR_MASK_BLUE) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
                if (colorMask & graphics::ColorMask::COLOR_MASK_ALPHA) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
                blendStateDesc.RenderTarget[0] = targetBlendDesc;

                if (blendState) blendState->Release();

                HRESULT hr;
                if (FAILED(hr = renderDevice.getDevice()->CreateBlendState(&blendStateDesc, &blendState)))
                    throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 blend state");
            }

            BlendState::~BlendState()
            {
                if (blendState)
                    blendState->Release();
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
