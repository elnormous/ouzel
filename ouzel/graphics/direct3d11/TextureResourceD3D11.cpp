// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include "TextureResourceD3D11.hpp"
#include "RenderDeviceD3D11.hpp"

namespace ouzel
{
    namespace graphics
    {
        static DXGI_FORMAT getD3D11PixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM: return DXGI_FORMAT_A8_UNORM;
                case PixelFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
                case PixelFormat::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
                case PixelFormat::R8_UINT: return DXGI_FORMAT_R8_UINT;
                case PixelFormat::R8_SINT: return DXGI_FORMAT_R8_SINT;
                case PixelFormat::R16_UNORM: return DXGI_FORMAT_R16_UNORM;
                case PixelFormat::R16_SNORM: return DXGI_FORMAT_R16_SNORM;
                case PixelFormat::R16_UINT: return DXGI_FORMAT_R16_UINT;
                case PixelFormat::R16_SINT: return DXGI_FORMAT_R16_SINT;
                case PixelFormat::R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
                case PixelFormat::R32_UINT: return DXGI_FORMAT_R32_UINT;
                case PixelFormat::R32_SINT: return DXGI_FORMAT_R32_SINT;
                case PixelFormat::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
                case PixelFormat::RG8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
                case PixelFormat::RG8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
                case PixelFormat::RG8_UINT: return DXGI_FORMAT_R8G8_UINT;
                case PixelFormat::RG8_SINT: return DXGI_FORMAT_R8G8_SINT;
                case PixelFormat::RGBA8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
                case PixelFormat::RGBA8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
                case PixelFormat::RGBA8_UINT: return DXGI_FORMAT_R8G8B8A8_UINT;
                case PixelFormat::RGBA8_SINT: return DXGI_FORMAT_R8G8B8A8_SINT;
                case PixelFormat::RGBA16_UNORM: return DXGI_FORMAT_R16G16B16A16_UNORM;
                case PixelFormat::RGBA16_SNORM: return DXGI_FORMAT_R16G16B16A16_SNORM;
                case PixelFormat::RGBA16_UINT: return DXGI_FORMAT_R16G16B16A16_UINT;
                case PixelFormat::RGBA16_SINT: return DXGI_FORMAT_R16G16B16A16_SINT;
                case PixelFormat::RGBA16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
                case PixelFormat::RGBA32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
                case PixelFormat::RGBA32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;
                case PixelFormat::RGBA32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }

        TextureResourceD3D11::TextureResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11,
                                                   const std::vector<Texture::Level>& levels,
                                                   uint32_t newFlags,
                                                   uint32_t newSampleCount,
                                                   PixelFormat newPixelFormat):
            RenderResourceD3D11(renderDeviceD3D11),
            flags(newFlags),
            mipmaps(static_cast<uint32_t>(levels.size())),
            sampleCount(newSampleCount),
            pixelFormat(newPixelFormat)
        {
            if ((flags & Texture::RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            createTexture(levels);

            if (flags & Texture::RENDER_TARGET)
            {
                frameBufferClearColor[0] = clearColor.normR();
                frameBufferClearColor[1] = clearColor.normG();
                frameBufferClearColor[2] = clearColor.normB();
                frameBufferClearColor[3] = clearColor.normA();
            }

            samplerDescriptor.filter = renderDevice.getTextureFilter();
            samplerDescriptor.addressX = Texture::Address::CLAMP;
            samplerDescriptor.addressY = Texture::Address::CLAMP;
            samplerDescriptor.maxAnisotropy = renderDevice.getMaxAnisotropy();

            updateSamplerState();
        }

        TextureResourceD3D11::~TextureResourceD3D11()
        {
            if (depthStencilTexture)
                depthStencilTexture->Release();

            if (depthStencilView)
                depthStencilView->Release();

            if (renderTargetView)
                renderTargetView->Release();

            if (resourceView)
                resourceView->Release();

            if (texture)
                texture->Release();

            if (samplerState)
                samplerState->Release();
        }

        void TextureResourceD3D11::setData(const std::vector<Texture::Level>& levels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            RenderDeviceD3D11& renderDeviceD3D11 = static_cast<RenderDeviceD3D11&>(renderDevice);

            if (!texture)
                createTexture(levels);
            else if (!(flags & Texture::RENDER_TARGET))
            {
                if (flags & Texture::DYNAMIC)
                {
                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].data.empty())
                        {
                            D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                            mappedSubresource.pData = nullptr;
                            mappedSubresource.RowPitch = 0;
                            mappedSubresource.DepthPitch = 0;

                            HRESULT hr;
                            if (FAILED(hr = renderDeviceD3D11.getContext()->Map(texture, static_cast<UINT>(level),
                                                                                (level == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE,
                                                                                0, &mappedSubresource)))
                                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to map Direct3D 11 texture");

                            uint8_t* destination = static_cast<uint8_t*>(mappedSubresource.pData);

                            if (mappedSubresource.RowPitch == levels[level].pitch)
                            {
                                std::copy(levels[level].data.begin(),
                                          levels[level].data.end(),
                                          destination);
                            }
                            else
                            {
                                auto source = levels[level].data.begin();
                                UINT rowSize = static_cast<UINT>(levels[level].size.width) * getPixelSize(pixelFormat);
                                UINT rows = static_cast<UINT>(levels[level].size.height);

                                for (UINT row = 0; row < rows; ++row)
                                {
                                    std::copy(source,
                                              source + rowSize,
                                              destination);

                                    source += levels[level].pitch;
                                    destination += mappedSubresource.RowPitch;
                                }
                            }

                            renderDeviceD3D11.getContext()->Unmap(texture, static_cast<UINT>(level));
                        }
                    }
                }
                else
                {
                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].data.empty())
                        {
                            renderDeviceD3D11.getContext()->UpdateSubresource(texture, static_cast<UINT>(level),
                                                                              nullptr, levels[level].data.data(),
                                                                              static_cast<UINT>(levels[level].pitch), 0);
                        }
                    }
                }
            }
        }

        void TextureResourceD3D11::setFilter(Texture::Filter filter)
        {
            samplerDescriptor.filter = (filter == Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;
            updateSamplerState();
        }

        void TextureResourceD3D11::setAddressX(Texture::Address addressX)
        {
            samplerDescriptor.addressX = addressX;
            updateSamplerState();
        }

        void TextureResourceD3D11::setAddressY(Texture::Address addressY)
        {
            samplerDescriptor.addressY = addressY;
            updateSamplerState();
        }

        void TextureResourceD3D11::setMaxAnisotropy(uint32_t maxAnisotropy)
        {
            samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;
            updateSamplerState();
        }

        void TextureResourceD3D11::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            clearFrameBufferView = clearColorBuffer;
        }

        void TextureResourceD3D11::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            clearDepthBufferView = clearDepthBuffer;
        }

        void TextureResourceD3D11::setClearColor(Color color)
        {
            clearColor = color;

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();
        }

        void TextureResourceD3D11::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }

        void TextureResourceD3D11::createTexture(const std::vector<Texture::Level>& levels)
        {
            if (texture)
                texture->Release();

            if (resourceView)
            {
                resourceView->Release();
                resourceView = nullptr;
            }

            if (renderTargetView)
            {
                renderTargetView->Release();
                renderTargetView = nullptr;
            }

            if (depthStencilTexture)
            {
                depthStencilTexture->Release();
                depthStencilTexture = nullptr;
            }

            if (depthStencilView)
            {
                depthStencilView->Release();
                depthStencilView = nullptr;
            }

            width = static_cast<UINT>(levels.front().size.width);
            height = static_cast<UINT>(levels.front().size.height);

            if (width > 0 && height > 0)
            {
                DXGI_FORMAT d3d11PixelFormat = getD3D11PixelFormat(pixelFormat);

                if (d3d11PixelFormat == DXGI_FORMAT_UNKNOWN)
                    throw std::runtime_error("Invalid pixel format");

                D3D11_TEXTURE2D_DESC textureDescriptor;
                textureDescriptor.Width = width;
                textureDescriptor.Height = height;
                textureDescriptor.MipLevels = static_cast<UINT>(levels.size());
                textureDescriptor.ArraySize = 1;
                textureDescriptor.Format = d3d11PixelFormat;
                textureDescriptor.SampleDesc.Count = sampleCount;
                textureDescriptor.SampleDesc.Quality = 0;
                if (flags & Texture::RENDER_TARGET) textureDescriptor.Usage = D3D11_USAGE_DEFAULT;
                else if (flags & Texture::DYNAMIC) textureDescriptor.Usage = D3D11_USAGE_DYNAMIC;
                else textureDescriptor.Usage = D3D11_USAGE_IMMUTABLE;

                if (flags & Texture::RENDER_TARGET)
                {
                    textureDescriptor.BindFlags = D3D11_BIND_RENDER_TARGET;
                    if (flags & Texture::BINDABLE_COLOR_BUFFER) textureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
                }
                else
                    textureDescriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;

                textureDescriptor.CPUAccessFlags = (flags & Texture::DYNAMIC && !(flags & Texture::RENDER_TARGET)) ? D3D11_CPU_ACCESS_WRITE : 0;
                textureDescriptor.MiscFlags = 0;

                RenderDeviceD3D11& renderDeviceD3D11 = static_cast<RenderDeviceD3D11&>(renderDevice);

                if (levels.empty() || flags & Texture::RENDER_TARGET)
                {
                    HRESULT hr;
                    if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateTexture2D(&textureDescriptor, nullptr, &texture)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 texture");
                }
                else
                {
                    std::vector<D3D11_SUBRESOURCE_DATA> subresourceData(levels.size());

                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        subresourceData[level].pSysMem = levels[level].data.data();
                        subresourceData[level].SysMemPitch = static_cast<UINT>(levels[level].pitch);
                        subresourceData[level].SysMemSlicePitch = 0;
                    }

                    HRESULT hr;
                    if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateTexture2D(&textureDescriptor, subresourceData.data(), &texture)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 texture");
                }

                D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
                resourceViewDesc.Format = d3d11PixelFormat;
                resourceViewDesc.ViewDimension = (sampleCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
                resourceViewDesc.Texture2D.MostDetailedMip = 0;
                resourceViewDesc.Texture2D.MipLevels = (sampleCount == 1) ? static_cast<UINT>(levels.size()) : 0;

                HRESULT hr;
                if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 shader resource view");

                if (flags & Texture::RENDER_TARGET)
                {
                    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                    renderTargetViewDesc.Format = d3d11PixelFormat;
                    renderTargetViewDesc.ViewDimension = (sampleCount > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

                    if (sampleCount == 1)
                        renderTargetViewDesc.Texture2D.MipSlice = 0;

                    if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTargetView)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");

                    if (flags & Texture::DEPTH_BUFFER)
                    {
                        D3D11_TEXTURE2D_DESC depthStencilDesc;
                        depthStencilDesc.Width = width;
                        depthStencilDesc.Height = height;
                        depthStencilDesc.MipLevels = 1;
                        depthStencilDesc.ArraySize = 1;
                        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
                        depthStencilDesc.SampleDesc.Count = sampleCount;
                        depthStencilDesc.SampleDesc.Quality = 0;
                        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
                        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                        if (flags & Texture::BINDABLE_DEPTH_BUFFER) textureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

                        depthStencilDesc.CPUAccessFlags = 0;
                        depthStencilDesc.MiscFlags = 0;

                        if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture)))
                            throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil texture");

                        if (FAILED(hr = renderDeviceD3D11.getDevice()->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView)))
                            throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil view");
                    }
                }
            }
        }

        void TextureResourceD3D11::updateSamplerState()
        {
            RenderDeviceD3D11& renderDeviceD3D11 = static_cast<RenderDeviceD3D11&>(renderDevice);

            if (samplerState) samplerState->Release();
            samplerState = renderDeviceD3D11.getSamplerState(samplerDescriptor);

            if (!samplerState)
                throw std::runtime_error("Failed to get D3D11 sampler state");

            samplerState->AddRef();
        }
    } // namespace graphics
} // namespace ouzel

#endif
