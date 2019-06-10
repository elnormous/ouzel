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
            static D3D11_BLEND getBlendFactor(graphics::BlendState::Factor blendFactor)
            {
                switch (blendFactor)
                {
                    case graphics::BlendState::Factor::ZERO: return D3D11_BLEND_ZERO;
                    case graphics::BlendState::Factor::ONE: return D3D11_BLEND_ONE;
                    case graphics::BlendState::Factor::SRC_COLOR: return D3D11_BLEND_SRC_COLOR;
                    case graphics::BlendState::Factor::INV_SRC_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
                    case graphics::BlendState::Factor::SRC_ALPHA: return D3D11_BLEND_SRC_ALPHA;
                    case graphics::BlendState::Factor::INV_SRC_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
                    case graphics::BlendState::Factor::DEST_ALPHA: return D3D11_BLEND_DEST_ALPHA;
                    case graphics::BlendState::Factor::INV_DEST_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
                    case graphics::BlendState::Factor::DEST_COLOR: return D3D11_BLEND_DEST_COLOR;
                    case graphics::BlendState::Factor::INV_DEST_COLOR: return D3D11_BLEND_INV_DEST_COLOR;
                    case graphics::BlendState::Factor::SRC_ALPHA_SAT: return D3D11_BLEND_SRC_ALPHA_SAT;
                    case graphics::BlendState::Factor::BLEND_FACTOR: return D3D11_BLEND_BLEND_FACTOR;
                    case graphics::BlendState::Factor::INV_BLEND_FACTOR: return D3D11_BLEND_INV_BLEND_FACTOR;
                    default: return D3D11_BLEND_ZERO;
                }
            }

            static D3D11_BLEND_OP getBlendOperation(graphics::BlendState::Operation blendOperation)
            {
                switch (blendOperation)
                {
                    case graphics::BlendState::Operation::ADD: return D3D11_BLEND_OP_ADD;
                    case graphics::BlendState::Operation::SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
                    case graphics::BlendState::Operation::REV_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
                    case graphics::BlendState::Operation::MIN: return D3D11_BLEND_OP_MIN;
                    case graphics::BlendState::Operation::MAX: return D3D11_BLEND_OP_MAX;
                    default: return D3D11_BLEND_OP_ADD;
                }
            }

            BlendState::BlendState(RenderDevice& renderDevice,
                                   bool enableBlending,
                                   graphics::BlendState::Factor colorBlendSource,
                                   graphics::BlendState::Factor colorBlendDest,
                                   graphics::BlendState::Operation colorOperation,
                                   graphics::BlendState::Factor alphaBlendSource,
                                   graphics::BlendState::Factor alphaBlendDest,
                                   graphics::BlendState::Operation alphaOperation,
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
                if (colorMask & graphics::BlendState::COLOR_MASK_RED) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
                if (colorMask & graphics::BlendState::COLOR_MASK_GREEN) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
                if (colorMask & graphics::BlendState::COLOR_MASK_BLUE) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
                if (colorMask & graphics::BlendState::COLOR_MASK_ALPHA) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
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
