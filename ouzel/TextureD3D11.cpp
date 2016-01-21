// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Image.h"
#include "Utils.h"
#include "stb_image_resize.h"

namespace ouzel
{
    TextureD3D11::TextureD3D11()
    {
        
    }
    
    TextureD3D11::~TextureD3D11()
    {
        clean();
    }

    void TextureD3D11::clean()
    {
        if (_resourceView) _resourceView->Release();
        if (_texture) _texture->Release();
    }

    bool TextureD3D11::init(Size2 const& size, bool dynamic)
    {
        if (!Texture::init(size, dynamic))
        {
            return false;
        }

        clean();

        return createTexture(nullptr,
                             static_cast<UINT>(size.width),
                             static_cast<UINT>(size.height));
    }

    bool TextureD3D11::initFromData(const void* data, const Size2& size, bool dynamic)
    {
        if (!Texture::initFromData(data, size, dynamic))
        {
            return false;
        }

        clean();

        return createTexture(data,
                             static_cast<UINT>(size.width),
                             static_cast<UINT>(size.height));
    }

    bool TextureD3D11::upload(const void* data, const Size2& size)
    {
        if (!Texture::upload(data, size))
        {
            return false;
        }

        if (static_cast<UINT>(size.width) != _width ||
            static_cast<UINT>(size.height) != _height)
        {
            if (_resourceView) _resourceView->Release();
            if (_texture) _texture->Release();

            return createTexture(data,
                                 static_cast<UINT>(size.width),
                                 static_cast<UINT>(size.height));
        }
        else
        {
            return uploadData(data,
                              static_cast<UINT>(size.width),
                              static_cast<UINT>(size.height));
        }
    }

    bool TextureD3D11::createTexture(const void* data, UINT width, UINT height)
    {
        std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(Engine::getInstance()->getRenderer());

        D3D11_TEXTURE2D_DESC textureDesc;
        memset(&textureDesc, 0, sizeof(textureDesc));
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 0;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Usage = _dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        textureDesc.CPUAccessFlags = _dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // | D3D11_BIND_RENDER_TARGET;
        textureDesc.MiscFlags = 0;

        HRESULT hr = rendererD3D11->getDevice()->CreateTexture2D(&textureDesc, nullptr, &_texture);
        if (FAILED(hr) || !_texture)
        {
            log("Failed to create D3D11 texture");
            return false;
        }

        UINT rowPitch = width * 4;

        if (data)
        {
            rendererD3D11->getContext()->UpdateSubresource(_texture, 0, nullptr, data, rowPitch, 0);
        }
        else
        {
            std::unique_ptr<uint8_t[]> emptyData(new uint8_t[width * height * 4]);
            memset(emptyData.get(), 0, width * height * 4);
            rendererD3D11->getContext()->UpdateSubresource(_texture, 0, nullptr, emptyData.get(), rowPitch, 0);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        memset(&srvDesc, 0, sizeof(srvDesc));
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        hr = rendererD3D11->getDevice()->CreateShaderResourceView(_texture, &srvDesc, &_resourceView);
        if (FAILED(hr) || !_resourceView)
        {
            log("Failed to create D3D11 shader resource view");
            return false;
        }

        _width = width;
        _height = height;

        return true;
    }

    bool TextureD3D11::uploadData(const void* data, UINT width, UINT height)
    {
        std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(Engine::getInstance()->getRenderer());

        UINT rowPitch = static_cast<UINT>(_width * 4);
        rendererD3D11->getContext()->UpdateSubresource(_texture, 0, nullptr, data, rowPitch, 0);

        if (isPOT(width) && isPOT(height))
        {
            UINT mipWidth = width / 2;
            UINT mipHeight = height / 2;
            UINT mipLevel = 1;
            UINT mipRowPitch = mipWidth * 4;

            std::unique_ptr<uint8_t[]> mipMapData(new uint8_t[width * height * 4]);

            while (mipWidth && mipHeight)
            {
                stbir_resize_uint8(static_cast<const uint8_t*>(data), width, height, 0, mipMapData.get(), mipWidth, mipHeight, 0, 4);

                rendererD3D11->getContext()->UpdateSubresource(_texture, mipLevel, nullptr, mipMapData.get(), mipRowPitch, 0);

                mipWidth /= 2;
                mipHeight /= 2;
                mipLevel++;
                mipRowPitch = mipWidth * 4;
            }
        }

        return true;
    }
}
