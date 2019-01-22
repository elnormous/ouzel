// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <stdexcept>
#include "D3D11Texture.hpp"
#include "D3D11RenderDevice.hpp"

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
                case PixelFormat::DEPTH: return DXGI_FORMAT_D32_FLOAT;
                case PixelFormat::DEPTH_STENCIL: return DXGI_FORMAT_D24_UNORM_S8_UINT;
                default: return DXGI_FORMAT_UNKNOWN;
            }
        }

        D3D11Texture::D3D11Texture(D3D11RenderDevice& renderDeviceD3D11,
                                   const std::vector<Texture::Level>& levels,
                                   uint32_t initFlags,
                                   uint32_t initSampleCount,
                                   PixelFormat initPixelFormat):
            D3D11RenderResource(renderDeviceD3D11),
            flags(initFlags),
            mipmaps(static_cast<uint32_t>(levels.size())),
            sampleCount(initSampleCount),
            pixelFormat(getD3D11PixelFormat(initPixelFormat)),
            pixelSize(getPixelSize(initPixelFormat))
        {
            if ((flags & Texture::RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (pixelFormat == DXGI_FORMAT_UNKNOWN)
                throw std::runtime_error("Invalid pixel format");

            width = static_cast<UINT>(levels.front().size.v[0]);
            height = static_cast<UINT>(levels.front().size.v[1]);

            if (!width || !height)
                throw std::runtime_error("Invalid texture size");

            D3D11_TEXTURE2D_DESC textureDescriptor;
            textureDescriptor.Width = width;
            textureDescriptor.Height = height;
            textureDescriptor.MipLevels = static_cast<UINT>(levels.size());
            textureDescriptor.ArraySize = 1;
            textureDescriptor.Format = pixelFormat;
            textureDescriptor.SampleDesc.Count = 1;
            textureDescriptor.SampleDesc.Quality = 0;
            if (flags & Texture::RENDER_TARGET) textureDescriptor.Usage = D3D11_USAGE_DEFAULT;
            else if (flags & Texture::DYNAMIC) textureDescriptor.Usage = D3D11_USAGE_DYNAMIC;
            else textureDescriptor.Usage = D3D11_USAGE_IMMUTABLE;

            if (flags & Texture::RENDER_TARGET)
            {
                textureDescriptor.BindFlags = (sampleCount == 1) ? D3D11_BIND_RENDER_TARGET : 0;
                if (flags & Texture::BINDABLE) textureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
            }
            else
                textureDescriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            textureDescriptor.CPUAccessFlags = (flags & Texture::DYNAMIC && !(flags & Texture::RENDER_TARGET)) ? D3D11_CPU_ACCESS_WRITE : 0;
            textureDescriptor.MiscFlags = 0;

            if (levels.empty() || flags & Texture::RENDER_TARGET)
            {
                HRESULT hr;
                if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&textureDescriptor, nullptr, &texture)))
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
                if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&textureDescriptor, subresourceData.data(), &texture)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 texture");
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
            resourceViewDesc.Format = pixelFormat;
            resourceViewDesc.ViewDimension = (sampleCount > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
            resourceViewDesc.Texture2D.MostDetailedMip = 0;
            resourceViewDesc.Texture2D.MipLevels = (sampleCount == 1) ? static_cast<UINT>(levels.size()) : 0;

            HRESULT hr;
            if (FAILED(hr = renderDevice.getDevice()->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView)))
                throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 shader resource view");

            if (flags & Texture::RENDER_TARGET)
            {
                D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                renderTargetViewDesc.Format = pixelFormat;
                renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                renderTargetViewDesc.Texture2D.MipSlice = 0;

                if (sampleCount > 1)
                {
                    D3D11_TEXTURE2D_DESC msaaTextureDescriptor;
                    msaaTextureDescriptor.Width = width;
                    msaaTextureDescriptor.Height = height;
                    msaaTextureDescriptor.MipLevels = 1;
                    msaaTextureDescriptor.ArraySize = 1;
                    msaaTextureDescriptor.Format = pixelFormat;
                    msaaTextureDescriptor.SampleDesc.Count = sampleCount;
                    msaaTextureDescriptor.SampleDesc.Quality = 0;
                    msaaTextureDescriptor.Usage = D3D11_USAGE_DEFAULT;
                    msaaTextureDescriptor.BindFlags = D3D11_BIND_RENDER_TARGET;
                    msaaTextureDescriptor.CPUAccessFlags = 0;
                    msaaTextureDescriptor.MiscFlags = 0;

                    if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&msaaTextureDescriptor, nullptr, &msaaTexture)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 texture");
                    
                    if (FAILED(hr = renderDevice.getDevice()->CreateRenderTargetView(msaaTexture, &renderTargetViewDesc, &renderTargetView)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");
                }
                else if (FAILED(hr = renderDevice.getDevice()->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTargetView)))
                    throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 render target view");

                if (flags & Texture::DEPTH_BUFFER)
                {
                    D3D11_TEXTURE2D_DESC depthStencilDescriptor;
                    depthStencilDescriptor.Width = width;
                    depthStencilDescriptor.Height = height;
                    depthStencilDescriptor.MipLevels = 1;
                    depthStencilDescriptor.ArraySize = 1;
                    depthStencilDescriptor.Format = DXGI_FORMAT_D32_FLOAT;
                    depthStencilDescriptor.SampleDesc.Count = sampleCount;
                    depthStencilDescriptor.SampleDesc.Quality = 0;
                    depthStencilDescriptor.Usage = D3D11_USAGE_DEFAULT;
                    depthStencilDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                    if (flags & Texture::BINDABLE) textureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

                    depthStencilDescriptor.CPUAccessFlags = 0;
                    depthStencilDescriptor.MiscFlags = 0;

                    if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&depthStencilDescriptor, nullptr, &depthStencilTexture)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil texture");

                    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
                    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
                    depthStencilViewDesc.ViewDimension = (sampleCount > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
                    depthStencilViewDesc.Flags = 0;

                    if (FAILED(hr = renderDevice.getDevice()->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &depthStencilView)))
                        throw std::system_error(hr, direct3D11ErrorCategory, "Failed to create Direct3D 11 depth stencil view");
                }
            }

            samplerDescriptor.filter = renderDevice.getTextureFilter();
            samplerDescriptor.addressX = Texture::Address::CLAMP;
            samplerDescriptor.addressY = Texture::Address::CLAMP;
            samplerDescriptor.maxAnisotropy = renderDevice.getMaxAnisotropy();

            updateSamplerState();
        }

        static D3D11_SRV_DIMENSION getShaderViewDimension(Texture::Dimensions dimensions)
        {
            switch (dimensions)
            {
                case Texture::Dimensions::ONE: return D3D11_SRV_DIMENSION_TEXTURE1D;
                case Texture::Dimensions::TWO: return D3D11_SRV_DIMENSION_TEXTURE2D;
                case Texture::Dimensions::THREE: return D3D11_SRV_DIMENSION_TEXTURE3D;
                case Texture::Dimensions::CUBE: return D3D11_SRV_DIMENSION_TEXTURE3D;
                default: throw std::runtime_error("Invalid texture type");
            }
        }

        D3D11Texture::~D3D11Texture()
        {
            if (depthStencilTexture)
                depthStencilTexture->Release();

            if (depthStencilView)
                depthStencilView->Release();

            if (renderTargetView)
                renderTargetView->Release();

            if (resourceView)
                resourceView->Release();

            if (msaaTexture)
                msaaTexture->Release();

            if (texture)
                texture->Release();

            if (samplerState)
                samplerState->Release();
        }

        void D3D11Texture::setData(const std::vector<Texture::Level>& levels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            if (!(flags & Texture::RENDER_TARGET))
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
                            if (FAILED(hr = renderDevice.getContext()->Map(texture, static_cast<UINT>(level),
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
                                uint32_t rowSize = static_cast<uint32_t>(levels[level].size.v[0]) * pixelSize;
                                UINT rows = static_cast<UINT>(levels[level].size.v[1]);

                                for (UINT row = 0; row < rows; ++row)
                                {
                                    std::copy(source,
                                              source + rowSize,
                                              destination);

                                    source += levels[level].pitch;
                                    destination += mappedSubresource.RowPitch;
                                }
                            }

                            renderDevice.getContext()->Unmap(texture, static_cast<UINT>(level));
                        }
                    }
                }
                else
                {
                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].data.empty())
                            renderDevice.getContext()->UpdateSubresource(texture, static_cast<UINT>(level),
                                                                         nullptr, levels[level].data.data(),
                                                                         static_cast<UINT>(levels[level].pitch), 0);
                    }
                }
            }
        }

        void D3D11Texture::setFilter(Texture::Filter filter)
        {
            samplerDescriptor.filter = (filter == Texture::Filter::DEFAULT) ? renderDevice.getTextureFilter() : filter;
            updateSamplerState();
        }

        void D3D11Texture::setAddressX(Texture::Address addressX)
        {
            samplerDescriptor.addressX = addressX;
            updateSamplerState();
        }

        void D3D11Texture::setAddressY(Texture::Address addressY)
        {
            samplerDescriptor.addressY = addressY;
            updateSamplerState();
        }

        void D3D11Texture::setMaxAnisotropy(uint32_t maxAnisotropy)
        {
            samplerDescriptor.maxAnisotropy = (maxAnisotropy == 0) ? renderDevice.getMaxAnisotropy() : maxAnisotropy;
            updateSamplerState();
        }

        void D3D11Texture::setClearColorBuffer(bool clear)
        {
            clearFrameBufferView = clear;
        }

        void D3D11Texture::setClearDepthBuffer(bool clear)
        {
            clearDepthBufferView = clear;
        }

        void D3D11Texture::setClearColor(Color color)
        {
            frameBufferClearColor[0] = color.normR();
            frameBufferClearColor[1] = color.normG();
            frameBufferClearColor[2] = color.normB();
            frameBufferClearColor[3] = color.normA();
        }

        void D3D11Texture::setClearDepth(float newClearDepth)
        {
            clearDepth = newClearDepth;
        }

        void D3D11Texture::resolve()
        {
            if (msaaTexture)
                renderDevice.getContext()->ResolveSubresource(texture, 0, msaaTexture, 0, pixelFormat);
        }

        void D3D11Texture::updateSamplerState()
        {
            if (samplerState) samplerState->Release();
            samplerState = renderDevice.getSamplerState(samplerDescriptor);

            if (!samplerState)
                throw std::runtime_error("Failed to get D3D11 sampler state");

            samplerState->AddRef();
        }
    } // namespace graphics
} // namespace ouzel

#endif
