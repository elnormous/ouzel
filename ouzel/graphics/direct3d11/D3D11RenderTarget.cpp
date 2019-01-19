// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <stdexcept>
#include "D3D11RenderTarget.hpp"
#include "D3D11RenderDevice.hpp"
#include "D3D11Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        D3D11RenderTarget::D3D11RenderTarget(D3D11RenderDevice& renderDeviceD3D11):
            D3D11RenderResource(renderDeviceD3D11)
        {
        }

        D3D11RenderTarget::~D3D11RenderTarget()
        {
            if (depthStencilView)
                depthStencilView->Release();

            if (renderTargetView)
                renderTargetView->Release();
        }

        void D3D11RenderTarget::setColorTexture(D3D11Texture* texture)
        {
            if (renderTargetView)
                renderTargetView->Release();

            colorTexture = texture;

            if (texture)
            {
                D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                renderTargetViewDesc.Format = texture->getPixelFormat();
                renderTargetViewDesc.ViewDimension = (texture->getSampleCount() > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
                renderTargetViewDesc.Texture2D.MipSlice = 0;

                if (FAILED(hr = renderDevice.getDevice()->CreateRenderTargetView(texture->getTexture(), &renderTargetViewDesc, &renderTargetView)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");
            }
        }

        void D3D11RenderTarget::setDepthTexture(D3D11Texture* texture)
        {
            if (depthStencilView)
                depthStencilView->Release();

            depthTexture = texture;

            if (texture)
            {
                D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                renderTargetViewDesc.Format = texture->getPixelFormat();
                renderTargetViewDesc.ViewDimension = (texture->getSampleCount() > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
                renderTargetViewDesc.Texture2D.MipSlice = 0;

                if (FAILED(hr = renderDevice.getDevice()->CreateRenderTargetView(texture->getTexture(), &renderTargetViewDesc, &depthStencilView)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
