// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11BlendState.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel::graphics::d3d11
{
    namespace
    {
        constexpr D3D11_BLEND getBlendFactor(BlendFactor blendFactor)
        {
            switch (blendFactor)
            {
                case BlendFactor::zero: return D3D11_BLEND_ZERO;
                case BlendFactor::one: return D3D11_BLEND_ONE;
                case BlendFactor::srcColor: return D3D11_BLEND_SRC_COLOR;
                case BlendFactor::invSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
                case BlendFactor::srcAlpha: return D3D11_BLEND_SRC_ALPHA;
                case BlendFactor::invSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
                case BlendFactor::destAlpha: return D3D11_BLEND_DEST_ALPHA;
                case BlendFactor::invDestAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
                case BlendFactor::destColor: return D3D11_BLEND_DEST_COLOR;
                case BlendFactor::invDestColor: return D3D11_BLEND_INV_DEST_COLOR;
                case BlendFactor::srcAlphaSat: return D3D11_BLEND_SRC_ALPHA_SAT;
                case BlendFactor::blendFactor: return D3D11_BLEND_BLEND_FACTOR;
                case BlendFactor::invBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
                default: throw std::runtime_error{"Unsupported blend factor"};
            }
        }

        constexpr D3D11_BLEND_OP getBlendOperation(BlendOperation blendOperation)
        {
            switch (blendOperation)
            {
                case BlendOperation::add: return D3D11_BLEND_OP_ADD;
                case BlendOperation::subtract: return D3D11_BLEND_OP_SUBTRACT;
                case BlendOperation::reverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
                case BlendOperation::min: return D3D11_BLEND_OP_MIN;
                case BlendOperation::max: return D3D11_BLEND_OP_MAX;
                default: throw std::runtime_error{"Unsupported blend operation"};
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
                           ColorMask colorMask):
        RenderResource{initRenderDevice}
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
        if ((colorMask & ColorMask::red) == ColorMask::red) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
        if ((colorMask & ColorMask::green) == ColorMask::green) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
        if ((colorMask & ColorMask::blue) == ColorMask::blue) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
        if ((colorMask & ColorMask::alpha) == ColorMask::alpha) targetBlendDesc.RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
        blendStateDesc.RenderTarget[0] = targetBlendDesc;

        ID3D11BlendState* newBlendState;
        if (const auto hr = renderDevice.getDevice()->CreateBlendState(&blendStateDesc, &newBlendState); FAILED(hr))
            throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 blend state"};

        blendState = newBlendState;
    }
}

#endif
