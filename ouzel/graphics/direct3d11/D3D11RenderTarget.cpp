// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "D3D11RenderTarget.hpp"
#include "D3D11RenderDevice.hpp"
#include "D3D11Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        D3D11RenderTarget::D3D11RenderTarget(D3D11RenderDevice& renderDeviceD3D11,
                                             const std::set<D3D11Texture*>& initColorTextures,
                                             D3D11Texture* initDepthTexture):
            D3D11RenderResource(renderDeviceD3D11),
            colorTextures(initColorTextures),
            depthTexture(initDepthTexture)
        {
            for (D3D11Texture* colorTexture : colorTextures)
                if (colorTexture)
                    renderTargetViews.push_back(colorTexture->getRenderTargetView());

            depthStencilView = depthTexture ? depthTexture->getDepthStencilView() : nullptr;
        }

        D3D11RenderTarget::~D3D11RenderTarget()
        {
        }

        void D3D11RenderTarget::resolve()
        {
            for (D3D11Texture* colorTexture : colorTextures)
                colorTexture->resolve();

            if (depthTexture)
                depthTexture->resolve();
        }
    } // namespace graphics
} // namespace ouzel

#endif
