// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "Engine.h"
#include "RendererD3D11.h"
#include "Image.h"
#include "MathUtils.h"
#include "Utils.h"
#include "stb_image_resize.h"

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

            return createTexture(nullptr,
                                 static_cast<UINT>(size.width),
                                 static_cast<UINT>(size.height));
        }

        bool TextureD3D11::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromData(data, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            return createTexture(data,
                                 static_cast<UINT>(size.width),
                                 static_cast<UINT>(size.height));
        }

        bool TextureD3D11::upload(const void* data, const Size2& newSize)
        {
            if (!Texture::upload(data, newSize))
            {
                return false;
            }

            if (static_cast<UINT>(size.width) != width ||
                static_cast<UINT>(size.height) != height)
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

        bool TextureD3D11::createTexture(const void* data, UINT newWidth, UINT newHeight)
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

            if (data)
            {
				if (!uploadData(data, newWidth, newHeight))
				{
					return false;
				}
            }
            else
            {
                std::unique_ptr<uint8_t[]> emptyData(new uint8_t[newWidth * newHeight * 4]);
                memset(emptyData.get(), 0, newWidth * newHeight * 4);
				if (!uploadData(emptyData.get(), newWidth, newHeight))
				{
					return false;
				}
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

        bool TextureD3D11::uploadData(const void* data, UINT newWidth, UINT newHeight)
        {
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            UINT rowPitch = newWidth * 4;
            rendererD3D11->getContext()->UpdateSubresource(texture, 0, nullptr, data, rowPitch, 0);

            mipLevels = 1;

            if (mipmaps)
            {
                UINT oldMipWidth = newWidth;
                UINT oldMipHeight = newHeight;

                UINT mipWidth = newWidth >> 1;
                UINT mipHeight = newHeight >> 1;
                if (mipWidth < 1) mipWidth = 1;
                if (mipHeight < 1) mipHeight = 1;
    
                UINT mipLevel = 1;
                UINT mipRowPitch;

                std::vector<uint8_t> oldMipMapData(newWidth * newHeight * 4);
                memcpy(oldMipMapData.data(), data, newWidth * newHeight * 4);

                std::vector<uint8_t> newMipMapData(mipWidth * mipHeight * 4);

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;
                    mipRowPitch = mipWidth * 4;

                    stbir_resize_uint8_generic(oldMipMapData.data(), oldMipWidth, oldMipHeight, 0,
                                               newMipMapData.data(), mipWidth, mipHeight, 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    rendererD3D11->getContext()->UpdateSubresource(texture, mipLevel, nullptr, newMipMapData.data(), mipRowPitch, 0);

                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;

                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    mipLevel++;

                    newMipMapData.swap(oldMipMapData);
                }

                mipLevels = mipLevel;
            }

			width = newWidth;
			height = newHeight;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
