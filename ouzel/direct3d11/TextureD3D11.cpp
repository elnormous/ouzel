// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Image.h"
#include "MathUtils.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        TextureD3D11::TextureD3D11()
        {

        }

        TextureD3D11::~TextureD3D11()
        {
            destroy();
        }

        void TextureD3D11::destroy()
        {
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

        bool TextureD3D11::init(const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::init(newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            return createTexture(static_cast<UINT>(size.width),
                                 static_cast<UINT>(size.height));
        }

        bool TextureD3D11::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromData(data, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            if (!createTexture(static_cast<UINT>(size.width),
                               static_cast<UINT>(size.height)))
            {
                return false;
            }

            return uploadData(data, newSize);
        }

        bool TextureD3D11::uploadData(const void* data, const Size2& newSize)
        {
            if (static_cast<UINT>(newSize.width) != width ||
                static_cast<UINT>(newSize.height) != height)
            {
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

                if (!createTexture(static_cast<UINT>(size.width),
                                   static_cast<UINT>(size.height)))
                {
                    return false;
                }
            }

            return Texture::uploadData(data, newSize);
        }
        
        bool TextureD3D11::uploadMipmap(uint32_t level, const void* data)
        {
            if (!Texture::uploadMipmap(level, data))
            {
                return false;
            }

            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            UINT newWidth = static_cast<UINT>(newSize.width);

            UINT rowPitch = newWidth * 4;
            rendererD3D11->getContext()->UpdateSubresource(texture, level, nullptr, data, rowPitch, 0);

            return true;
        }

        bool TextureD3D11::createTexture(UINT newWidth, UINT newHeight)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            D3D11_TEXTURE2D_DESC textureDesc;
            memset(&textureDesc, 0, sizeof(textureDesc));
            textureDesc.Width = newWidth;
            textureDesc.Height = newHeight;
            textureDesc.MipLevels = 0;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
            textureDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // | D3D11_BIND_RENDER_TARGET;
            textureDesc.MiscFlags = 0;

            HRESULT hr = rendererD3D11->getDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
            if (FAILED(hr) || !texture)
            {
                log("Failed to create D3D11 texture");
                return false;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            memset(&srvDesc, 0, sizeof(srvDesc));
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = mipLevels;

            hr = rendererD3D11->getDevice()->CreateShaderResourceView(texture, &srvDesc, &resourceView);
            if (FAILED(hr) || !resourceView)
            {
                log("Failed to create D3D11 shader resource view");
                return false;
            }

            width = newWidth;
            height = newHeight;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
