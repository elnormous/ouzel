// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureD3D11.h"
#include "core/Engine.h"
#include "RendererD3D11.h"
#include "graphics/Image.h"
#include "math/MathUtils.h"
#include "utils/Utils.h"

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

        bool TextureD3D11::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget)
        {
            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            free();

            return createTexture(static_cast<UINT>(size.width),
                                 static_cast<UINT>(size.height));
        }

        bool TextureD3D11::initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            free();

            if (!createTexture(static_cast<UINT>(size.width),
                               static_cast<UINT>(size.height)))
            {
                return false;
            }

            ready = true;

            return uploadData(newData, newSize);
        }

        bool TextureD3D11::uploadData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (static_cast<UINT>(newSize.width) != width ||
                static_cast<UINT>(newSize.height) != height)
            {
                free();

                if (!createTexture(static_cast<UINT>(size.width),
                                   static_cast<UINT>(size.height)))
                {
                    return false;
                }

                ready = true;
            }

            if (!Texture::uploadData(newData, newSize))
            {
                return false;
            }

            return true;
        }

        bool TextureD3D11::uploadMipmap(uint32_t level, const std::vector<uint8_t>& newData)
        {
            if (!Texture::uploadMipmap(level, newData))
            {
                return false;
            }

            std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

            UINT newWidth = static_cast<UINT>(mipmapSizes[level].width);

            UINT rowPitch = newWidth * 4;
            rendererD3D11->getContext()->UpdateSubresource(texture, level, nullptr, newData.data(), rowPitch, 0);

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
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (renderTarget ? D3D11_BIND_RENDER_TARGET : 0);
            textureDesc.MiscFlags = 0;

            HRESULT hr = rendererD3D11->getDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
            if (FAILED(hr))
            {
                log("Failed to create D3D11 texture");
                return false;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            memset(&srvDesc, 0, sizeof(srvDesc));
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = mipmaps ? calculateMipLevels(newWidth, newHeight) : 1;

            hr = rendererD3D11->getDevice()->CreateShaderResourceView(texture, &srvDesc, &resourceView);
            if (FAILED(hr))
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
