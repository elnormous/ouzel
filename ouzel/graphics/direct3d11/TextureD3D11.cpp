// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        TextureD3D11::TextureD3D11()
        {
        }

        TextureD3D11::~TextureD3D11()
        {
            if (renderTargetView)
            {
                renderTargetView->Release();
            }

            if (resourceView)
            {
                resourceView->Release();
            }

            if (texture)
            {
                texture->Release();
            }

            width = 0;
            height = 0;
        }

        bool TextureD3D11::upload()
        {
            if (!TextureResource::upload())
            {
                return false;
            }

            if (data.dirty)
            {
                RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(sharedEngine->getRenderer());

                if (data.size.v[0] > 0 &&
                    data.size.v[1] > 0)
                {
                    if (!texture ||
                        static_cast<UINT>(data.size.v[0]) != width ||
                        static_cast<UINT>(data.size.v[1]) != height)
                    {
                        if (texture) texture->Release();

                        width = static_cast<UINT>(data.size.v[0]);
                        height = static_cast<UINT>(data.size.v[1]);

                        D3D11_TEXTURE2D_DESC textureDesc;
                        textureDesc.Width = width;
                        textureDesc.Height = height;
                        textureDesc.MipLevels = data.mipmaps ? 0 : 1;
                        textureDesc.ArraySize = 1;
                        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        textureDesc.Usage = (data.dynamic && !data.renderTarget) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                        textureDesc.CPUAccessFlags = (data.dynamic && !data.renderTarget) ? D3D11_CPU_ACCESS_WRITE : 0;
                        textureDesc.SampleDesc.Count = data.sampleCount;
                        textureDesc.SampleDesc.Quality = 0;
                        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (data.renderTarget ? D3D11_BIND_RENDER_TARGET : 0);
                        textureDesc.MiscFlags = 0;

                        HRESULT hr = rendererD3D11->getDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to create Direct3D 11 texture";
                            return false;
                        }

                        D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
                        resourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        resourceViewDesc.ViewDimension = (data.sampleCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

                        if (data.sampleCount == 1)
                        {
                            resourceViewDesc.Texture2D.MostDetailedMip = 0;
                            resourceViewDesc.Texture2D.MipLevels = data.mipmaps ? static_cast<UINT>(data.levels.size()) : 1;
                        }

                        hr = rendererD3D11->getDevice()->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to create Direct3D 11 shader resource view";
                            return false;
                        }

                        if (data.renderTarget)
                        {
                            if (!renderTargetView)
                            {
                                D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                                renderTargetViewDesc.Format = textureDesc.Format;
                                renderTargetViewDesc.ViewDimension = (data.sampleCount > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

                                if (data.sampleCount == 1)
                                {
                                    renderTargetViewDesc.Texture2D.MipSlice = 0;
                                }

                                HRESULT hr = rendererD3D11->getDevice()->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTargetView);
                                if (FAILED(hr))
                                {
                                    Log(Log::Level::ERR) << "Failed to create Direct3D 11 render target view";
                                    return false;
                                }
                            }

                            frameBufferClearColor[0] = data.clearColor.normR();
                            frameBufferClearColor[1] = data.clearColor.normG();
                            frameBufferClearColor[2] = data.clearColor.normB();
                            frameBufferClearColor[3] = data.clearColor.normA();
                        }

                        if (data.depth)
                        {
                            D3D11_TEXTURE2D_DESC depthStencilDesc;
                            depthStencilDesc.Width = width;
                            depthStencilDesc.Height = height;
                            depthStencilDesc.MipLevels = 1;
                            depthStencilDesc.ArraySize = 1;
                            depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
                            depthStencilDesc.SampleDesc.Count = data.sampleCount;
                            depthStencilDesc.SampleDesc.Quality = 0;
                            depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
                            depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                            depthStencilDesc.CPUAccessFlags = 0;
                            depthStencilDesc.MiscFlags = 0;
                            hr = rendererD3D11->getDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil texture";
                                return false;
                            }

                            hr = rendererD3D11->getDevice()->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView);
                            if (FAILED(hr))
                            {
                                Log(Log::Level::ERR) << "Failed to create Direct3D 11 depth stencil view";
                                return false;
                            }
                        }
                    }

                    for (size_t level = 0; level < data.levels.size(); ++level)
                    {
                        if (!data.levels[level].data.empty())
                        {
                            rendererD3D11->getContext()->UpdateSubresource(texture, static_cast<UINT>(level),
                                                                           nullptr, data.levels[level].data.data(),
                                                                           static_cast<UINT>(data.levels[level].pitch), 0);
                        }
                    }
                }

                data.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
