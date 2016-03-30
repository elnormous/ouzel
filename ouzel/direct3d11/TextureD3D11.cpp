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
    namespace video
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

        bool TextureD3D11::init(const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::init(size, dynamic, mipmaps))
            {
                return false;
            }

            clean();

            return createTexture(nullptr,
                                 static_cast<UINT>(size.width),
                                 static_cast<UINT>(size.height));
        }

        bool TextureD3D11::initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::initFromData(data, size, dynamic, mipmaps))
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
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

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

            if (data)
            {
                uploadData(data, width, height);
            }
            else
            {
                std::unique_ptr<uint8_t[]> emptyData(new uint8_t[width * height * 4]);
                memset(emptyData.get(), 0, width * height * 4);
                uploadData(emptyData.get(), width, height);
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            memset(&srvDesc, 0, sizeof(srvDesc));
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = _mipLevels;

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
            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            UINT rowPitch = static_cast<UINT>(width * 4);
            rendererD3D11->getContext()->UpdateSubresource(_texture, 0, nullptr, data, rowPitch, 0);

            _mipLevels = 1;

            if (_mipmaps)
            {
                UINT oldMipWidth = width;
                UINT oldMipHeight = height;

                UINT mipWidth = width >> 1;
                UINT mipHeight = height >> 1;
                UINT mipLevel = 1;
                UINT mipRowPitch = mipWidth * 4;

                uint8_t* oldMipMapData = new uint8_t[width * height * 4];
                memcpy(oldMipMapData, data, width * height * 4);

                uint8_t* newMipMapData = new uint8_t[mipWidth * mipHeight * 4];

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;

                    stbir_resize_uint8_generic(oldMipMapData, oldMipWidth, oldMipHeight, 0,
                                               newMipMapData, mipWidth, mipHeight, 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    rendererD3D11->getContext()->UpdateSubresource(_texture, mipLevel, nullptr, newMipMapData, mipRowPitch, 0);

                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;

                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    mipLevel++;
                    mipRowPitch = mipWidth * 4;

                    uint8_t* temp = oldMipMapData;
                    oldMipMapData = newMipMapData;
                    newMipMapData = temp;
                }

                delete [] oldMipMapData;
                delete [] newMipMapData;

                _mipLevels = mipLevel;
            }

            return true;
        }
    } // namespace video
} // namespace ouzel
