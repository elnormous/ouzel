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
        TextureD3D11::TextureD3D11():
            dirty(false)
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
            std::lock_guard<std::mutex> lock(dataMutex);

            Texture::free();

            data.clear();

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
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool TextureD3D11::initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return uploadData(newData, newSize);
        }

        bool TextureD3D11::upload(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            data.clear();

            if (!Texture::upload(newData, newSize))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool TextureD3D11::uploadData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!Texture::uploadData(newData, newSize))
            {
                return false;
            }

            return true;
        }

        bool TextureD3D11::uploadMipmap(uint32_t level, const Size2& mipMapSize, const std::vector<uint8_t>& newData)
        {
            if (!Texture::uploadMipmap(level, mipMapSize, newData))
            {
                return false;
            }

            if (data.size() < level + 1) data.resize(level + 1);

            data[level].width = static_cast<UINT>(mipMapSize.width);
            data[level].height = static_cast<UINT>(mipMapSize.height);
            data[level].data = newData;

            return true;
        }

        bool TextureD3D11::update()
        {
            if (dirty)
            {
                std::vector<Data> localData;
                Size2 localSize;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    localData = data;
                    localSize = size;
                }

                std::shared_ptr<RendererD3D11> rendererD3D11 = std::static_pointer_cast<RendererD3D11>(sharedEngine->getRenderer());

                if (localSize.width > 0 && localSize.height > 0)
                {
                    if (!texture ||
                        static_cast<UINT>(localSize.width) != width ||
                        static_cast<UINT>(localSize.height) != height)
                    {
                        if (texture) texture->Release();

                        width = static_cast<UINT>(localSize.width);
                        height = static_cast<UINT>(localSize.height);

                        D3D11_TEXTURE2D_DESC textureDesc;
                        memset(&textureDesc, 0, sizeof(textureDesc));
                        textureDesc.Width = width;
                        textureDesc.Height = height;
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
                            log("Failed to create Direct3D 11 texture");
                            return false;
                        }

                        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
                        memset(&srvDesc, 0, sizeof(srvDesc));
                        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                        srvDesc.Texture2D.MostDetailedMip = 0;
                        srvDesc.Texture2D.MipLevels = mipmaps ? static_cast<UINT>(localData.size()) : 1;

                        hr = rendererD3D11->getDevice()->CreateShaderResourceView(texture, &srvDesc, &resourceView);
                        if (FAILED(hr))
                        {
                            log("Failed to create Direct3D 11 shader resource view");
                            return false;
                        }
                    }

                    if (!localData.empty())
                    {
                        for (size_t level = 0; level < localData.size(); ++level)
                        {
                            UINT rowPitch = localData[level].width * 4;
                            rendererD3D11->getContext()->UpdateSubresource(texture, static_cast<UINT>(level), nullptr, localData[level].data.data(), rowPitch, 0);
                        }
                    }
                }

                ready = (texture != nullptr);
                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
