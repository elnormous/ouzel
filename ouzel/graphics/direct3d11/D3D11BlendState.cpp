// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11BlendState.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        static D3D11_BLEND getBlendFactor(BlendState::Factor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendState::Factor::ZERO: return D3D11_BLEND_ZERO;
                case BlendState::Factor::ONE: return D3D11_BLEND_ONE;
                case BlendState::Factor::SRC_COLOR: return D3D11_BLEND_SRC_COLOR;
                case BlendState::Factor::INV_SRC_COLOR: return D3D11_BLEND_INV_SRC_COLOR;
                case BlendState::Factor::SRC_ALPHA: return D3D11_BLEND_SRC_ALPHA;
                case BlendState::Factor::INV_SRC_ALPHA: return D3D11_BLEND_INV_SRC_ALPHA;
                case BlendState::Factor::DEST_ALPHA: return D3D11_BLEND_DEST_ALPHA;
                case BlendState::Factor::INV_DEST_ALPHA: return D3D11_BLEND_INV_DEST_ALPHA;
                case BlendState::Factor::DEST_COLOR: return D3D11_BLEND_DEST_COLOR;
                case BlendState::Factor::INV_DEST_COLOR: return D3D11_BLEND_INV_DEST_COLOR;
                case BlendState::Factor::SRC_ALPHA_SAT: return D3D11_BLEND_SRC_ALPHA_SAT;
                case BlendState::Factor::BLEND_FACTOR: return D3D11_BLEND_BLEND_FACTOR;
                case BlendState::Factor::INV_BLEND_FACTOR: return D3D11_BLEND_INV_BLEND_FACTOR;
                default: return D3D11_BLEND_ZERO;
            }
        }

        static D3D11_BLEND_OP getBlendOperation(BlendState::Operation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendState::Operation::ADD: return D3D11_BLEND_OP_ADD;
                case BlendState::Operation::SUBTRACT: return D3D11_BLEND_OP_SUBTRACT;
                case BlendState::Operation::REV_SUBTRACT: return D3D11_BLEND_OP_REV_SUBTRACT;
                case BlendState::Operation::MIN: return D3D11_BLEND_OP_MIN;
                case BlendState::Operation::MAX: return D3D11_BLEND_OP_MAX;
                default: return D3D11_BLEND_OP_ADD;
            }
        }

        D3D11BlendState::D3D11BlendState(D3D11RenderDevice& renderDeviceD3D11,
                                         bool enableBlending,
                                         BlendState::Factor colorBlendSource, BlendState::Factor colorBlendDest,
                                         BlendState::Operation colorOperation,
                                         BlendState::Factor alphaBlendSource, BlendState::Factor alphaBlendDest,
                                         BlendState::Operation alphaOperation,
                                         uint8_t colorMask):
            D3D11RenderResource(renderDeviceD3D11)
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
            if (colorMask & BlendState::COLOR_MASK_RED) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
            if (colorMask & BlendState::COLOR_MASK_GREEN) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
            if (colorMask & BlendState::COLOR_MASK_BLUE) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
            if (colorMask & BlendState::COLOR_MASK_ALPHA) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
            blendStateDesc.RenderTarget[0] = targetBlendDesc;

            if (blendState) blendState->Release();

            HRESULT hr;
            if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateBlendState(&blendStateDesc, &blendState)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 blend state");
        }

        D3D11BlendState::~D3D11BlendState()
        {
            if (blendState)
                blendState->Release();
        }
    } // namespace graphics
} // namespace ouzel

#endif
