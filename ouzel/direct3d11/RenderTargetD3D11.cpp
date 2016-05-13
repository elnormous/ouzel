// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetD3D11.h"
#include "TextureD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetD3D11::RenderTargetD3D11()
        {

        }

        RenderTargetD3D11::~RenderTargetD3D11()
        {
            free();
        }

        void RenderTargetD3D11::free()
        {
            if (renderTargetView)
            {
                renderTargetView->Release();
                renderTargetView = nullptr;
            }
        }

        bool RenderTargetD3D11::init(const Size2& newSize, bool depthBuffer)
        {
            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            free();

            viewport = { 0, 0, size.width, size.height, 0.0f, 1.0f };

            std::shared_ptr<TextureD3D11> textureD3D11(new TextureD3D11());

            if (!textureD3D11->init(size, false, false, true))
            {
                return false;
            }

            texture = textureD3D11;

            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_TEXTURE2D_DESC textureDesc;
            textureD3D11->texture->GetDesc(&textureDesc);

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            memset(&renderTargetViewDesc, 0, sizeof(renderTargetViewDesc));
            renderTargetViewDesc.Format = textureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            HRESULT hr = rendererD3D11->getDevice()->CreateRenderTargetView(textureD3D11->getTexture(), &renderTargetViewDesc, &renderTargetView);
            if (FAILED(hr))
            {
                log("Failed to create D3D11 render target view");
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
