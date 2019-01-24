// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <algorithm>
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
        }

        void D3D11RenderTarget::addColorTexture(D3D11Texture* texture)
        {
            if (texture && colorTextures.insert(texture).second)
                renderTargetViews.push_back(texture->getRenderTargetView());
        }

        void D3D11RenderTarget::removeColorTexture(D3D11Texture* texture)
        {
            auto i = colorTextures.find(texture);

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                auto renderTargetViewIterator = std::find(renderTargetViews.begin(), renderTargetViews.end(), texture->getRenderTargetView());
                if (renderTargetViewIterator != renderTargetViews.end())
                    renderTargetViews.erase(renderTargetViewIterator);
            }
        }

        void D3D11RenderTarget::setDepthTexture(D3D11Texture* texture)
        {
            depthTexture = texture;

            depthStencilView = texture ? texture->getDepthStencilView() : nullptr;
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
