// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Image.h"
#include "Utils.h"

namespace ouzel
{
    TextureD3D11::TextureD3D11()
    {
        
    }
    
    TextureD3D11::~TextureD3D11()
    {
        if (_resourceView) _resourceView->Release();
        if (_texture) _texture->Release();
    }

    bool TextureD3D11::initFromFile(const std::string& filename)
    {
        if (!Texture::initFromFile(filename))
        {
            return false;
        }

        AutoPtr<Image> image = new Image();
        if (!image->loadFromFile(filename))
        {
            return false;
        }

        RendererD3D11* rendererD3D11 = static_cast<RendererD3D11*>(Renderer::getInstance());
        int width = (int)image->getSize().width;
        int height = (int)image->getSize().height;

        D3D11_TEXTURE2D_DESC textureDesc;
        memset(&textureDesc, 0, sizeof(textureDesc));
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 0;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        HRESULT hr = rendererD3D11->getDevice()->CreateTexture2D(&textureDesc, NULL, &_texture);
        if (FAILED(hr) || !_texture)
        {
            log("Could not create D3D11 texture (type=2D, width=%d, height=%d, name=%s)", width, height, filename.c_str());
            return false;
        }

        UINT rowPitch = static_cast<UINT>(width * 4);
        rendererD3D11->getContext()->UpdateSubresource(_texture, 0, NULL, image->getData(), rowPitch, 0);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        memset(&srvDesc, 0, sizeof(srvDesc));
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = -1;

        hr = rendererD3D11->getDevice()->CreateShaderResourceView(_texture, &srvDesc, &_resourceView);
        if (FAILED(hr) || !_resourceView)
        {
            log("Could not create D3D11 shader resource view (type=2D, width=%d, height=%d, name=%s)", width, height, filename.c_str());
            return false;
        }

        rendererD3D11->getContext()->GenerateMips(_resourceView);

        _size = image->getSize();

        return true;
    }
}
