// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetD3D11.h"
#include "TextureD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetD3D11::RenderTargetD3D11()
        {

        }

        RenderTargetD3D11::~RenderTargetD3D11()
        {
            destroy();
        }

        void RenderTargetD3D11::destroy()
        {
            if (_renderTargetView)
            {
                _renderTargetView->Release();
                _renderTargetView = nullptr;
            }
        }

        bool RenderTargetD3D11::init(const Size2& size, bool depthBuffer)
        {
            if (!RenderTarget::init(size, depthBuffer))
            {
                return false;
            }

            destroy();

            std::shared_ptr<TextureD3D11> textureD3D11(new TextureD3D11());

            if (!textureD3D11->init(size, false))
            {
                return false;
            }

            _texture = textureD3D11;

            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: use this in D3D11Texture
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            HRESULT hr = rendererD3D11->getDevice()->CreateRenderTargetView(textureD3D11->getTexture(), &renderTargetViewDesc, &_renderTargetView);
            if (FAILED(hr))
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
