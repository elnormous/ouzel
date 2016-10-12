// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetD3D11.h"
#include "TextureD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetD3D11::RenderTargetD3D11()
        {
        }

        RenderTargetD3D11::~RenderTargetD3D11()
        {
            if (renderTargetView)
            {
                renderTargetView->Release();
            }
        }

        void RenderTargetD3D11::free()
        {
            RenderTarget::free();

            if (renderTargetView)
            {
                renderTargetView->Release();
                renderTargetView = nullptr;
            }
        }

        bool RenderTargetD3D11::upload()
        {
            if (uploadData.dirty)
            {
                if (!texture->upload())
                {
                    return false;
                }

                viewport = { 0, 0, uploadData.size.width, uploadData.size.height, 0.0f, 1.0f };

                std::shared_ptr<TextureD3D11> textureD3D11 = std::static_pointer_cast<TextureD3D11>(texture);

                if (!textureD3D11->getTexture())
                {
                    Log(Log::Level::ERR) << "Direct3D 11 texture not initialized";
                    return false;
                }

                if (!renderTargetView)
                {
                    D3D11_TEXTURE2D_DESC textureDesc;
                    textureD3D11->texture->GetDesc(&textureDesc);

                    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                    memset(&renderTargetViewDesc, 0, sizeof(renderTargetViewDesc));
                    renderTargetViewDesc.Format = textureDesc.Format;
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                    renderTargetViewDesc.Texture2D.MipSlice = 0;

                    std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

                    HRESULT hr = rendererD3D11->getDevice()->CreateRenderTargetView(textureD3D11->getTexture(), &renderTargetViewDesc, &renderTargetView);
                    if (FAILED(hr))
                    {
                        Log(Log::Level::ERR) << "Failed to create Direct3D 11 render target view";
                        return false;
                    }
                }

                frameBufferClearColor[0] = uploadData.clearColor.getR();
                frameBufferClearColor[1] = uploadData.clearColor.getG();
                frameBufferClearColor[2] = uploadData.clearColor.getB();
                frameBufferClearColor[3] = uploadData.clearColor.getA();

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
