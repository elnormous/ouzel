// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "graphics/Image.h"
#include "math/MathUtils.h"
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

        void TextureD3D11::free()
        {
            Texture::free();

            if (resourceView)
            {
                resourceView->Release();
                resourceView = nullptr;
            }

            if (texture)
            {
                texture->Release();
                texture = nullptr;
            }
        }

        bool TextureD3D11::upload()
        {
            if (uploadData.dirty)
            {
                std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

                if (uploadData.size.width > 0 &&
                    uploadData.size.height > 0)
                {
                    if (!texture ||
                        static_cast<UINT>(uploadData.size.width) != width ||
                        static_cast<UINT>(uploadData.size.height) != height)
                    {
                        if (texture) texture->Release();

                        width = static_cast<UINT>(uploadData.size.width);
                        height = static_cast<UINT>(uploadData.size.height);

                        D3D11_TEXTURE2D_DESC textureDesc;
                        memset(&textureDesc, 0, sizeof(textureDesc));
                        textureDesc.Width = width;
                        textureDesc.Height = height;
                        textureDesc.MipLevels = uploadData.mipmaps ? 0 : 1;
                        textureDesc.ArraySize = 1;
                        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        textureDesc.Usage = uploadData.dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
                        textureDesc.CPUAccessFlags = uploadData.dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
                        textureDesc.SampleDesc.Count = 1;
                        textureDesc.SampleDesc.Quality = 0;
                        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (uploadData.renderTarget ? D3D11_BIND_RENDER_TARGET : 0);
                        textureDesc.MiscFlags = 0;

                        HRESULT hr = rendererD3D11->getDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to create Direct3D 11 texture";
                            return false;
                        }

                        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                        memset(&srvDesc, 0, sizeof(srvDesc));
                        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                        srvDesc.Texture2D.MostDetailedMip = 0;
                        srvDesc.Texture2D.MipLevels = uploadData.mipmaps ? static_cast<UINT>(uploadData.levels.size()) : 1;

                        hr = rendererD3D11->getDevice()->CreateShaderResourceView(texture, &srvDesc, &resourceView);
                        if (FAILED(hr))
                        {
                            Log(Log::Level::ERR) << "Failed to create Direct3D 11 shader resource view";
                            return false;
                        }
                    }

                    for (size_t level = 0; level < uploadData.levels.size(); ++level)
                    {
                        rendererD3D11->getContext()->UpdateSubresource(texture, static_cast<UINT>(level),
                                                                       nullptr, uploadData.levels[level].data.data(),
                                                                       static_cast<UINT>(uploadData.levels[level].pitch), 0);
                    }
                }

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
