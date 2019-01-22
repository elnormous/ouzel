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
        D3D11RenderTarget::D3D11RenderTarget(D3D11RenderDevice& renderDeviceD3D11,
                                             bool initClearColorBuffer,
                                             bool initClearDepthBuffer,
                                             Color initClearColor,
                                             float initClearDepth):
            D3D11RenderResource(renderDeviceD3D11),
            clearFrameBufferView(initClearColorBuffer),
            clearDepthBufferView(initClearDepthBuffer),
            frameBufferClearColor{initClearColor.normR(),
                initClearColor.normG(),
                initClearColor.normB(),
                initClearColor.normA()},
            clearDepth(initClearDepth)
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

        void D3D11RenderTarget::setClearColorBuffer(bool clear)
        {
            clearFrameBufferView = clear;
        }

        void D3D11RenderTarget::setClearDepthBuffer(bool clear)
        {
            clearDepthBufferView = clear;
        }

        void D3D11RenderTarget::setClearColor(Color color)
        {
            frameBufferClearColor[0] = color.normR();
            frameBufferClearColor[1] = color.normG();
            frameBufferClearColor[2] = color.normB();
            frameBufferClearColor[3] = color.normA();
        }

        void D3D11RenderTarget::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }
    } // namespace graphics
} // namespace ouzel

#endif
