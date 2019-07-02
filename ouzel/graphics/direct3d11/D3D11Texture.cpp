// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <stdexcept>
#include "D3D11Texture.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            constexpr DXGI_FORMAT getPixelFormat(PixelFormat pixelFormat)
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
                    case PixelFormat::RGBA8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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

            constexpr D3D11_SRV_DIMENSION getShaderViewDimension(TextureType type, bool multisample)
            {
                if (multisample)
                {
                    switch (type)
                    {
                        case TextureType::TWO_DIMENSIONAL: return D3D11_SRV_DIMENSION_TEXTURE2DMS;
                        default: throw std::runtime_error("Invalid multisample texture type");
                    }
                }
                else
                {
                    switch (type)
                    {
                        case TextureType::ONE_DIMENSIONAL: return D3D11_SRV_DIMENSION_TEXTURE1D;
                        case TextureType::TWO_DIMENSIONAL: return D3D11_SRV_DIMENSION_TEXTURE2D;
                        case TextureType::THREE_DIMENSIONAL: return D3D11_SRV_DIMENSION_TEXTURE3D;
                        case TextureType::CUBE: return D3D11_SRV_DIMENSION_TEXTURE3D;
                        default: throw std::runtime_error("Invalid texture type");
                    }
                }
            }

            constexpr D3D11_TEXTURECUBE_FACE getCubeFace(CubeFace face)
            {
                switch (face)
                {
                    case CubeFace::POSITIVE_X: return D3D11_TEXTURECUBE_FACE_POSITIVE_X;
                    case CubeFace::NEGATIVE_X: return D3D11_TEXTURECUBE_FACE_NEGATIVE_X;
                    case CubeFace::POSITIVE_Y: return D3D11_TEXTURECUBE_FACE_POSITIVE_Y;
                    case CubeFace::NEGATIVE_Y: return D3D11_TEXTURECUBE_FACE_NEGATIVE_Y;
                    case CubeFace::POSITIVE_Z: return D3D11_TEXTURECUBE_FACE_POSITIVE_Z;
                    case CubeFace::NEGATIVE_Z: return D3D11_TEXTURECUBE_FACE_NEGATIVE_Z;
                    default: throw std::runtime_error("Invalid cube face");
                }
            }

            Texture::Texture(RenderDevice& initRenderDevice,
                             const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& levels,
                             TextureType type,
                             uint32_t initFlags,
                             uint32_t initSampleCount,
                             PixelFormat initPixelFormat):
                RenderResource(initRenderDevice),
                flags(initFlags),
                mipmaps(static_cast<uint32_t>(levels.size())),
                sampleCount(initSampleCount),
                pixelFormat(d3d11::getPixelFormat(initPixelFormat)),
                pixelSize(getPixelSize(initPixelFormat))
            {
                if ((flags & Flags::BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                    throw std::runtime_error("Invalid mip map count");

                if (pixelFormat == DXGI_FORMAT_UNKNOWN)
                    throw std::runtime_error("Invalid pixel format");

                DXGI_FORMAT texturePixelFormat = pixelFormat;
                DXGI_FORMAT shaderViewPixelFormat = pixelFormat;

                if (flags & Flags::BIND_SHADER || flags & Flags::BIND_SHADER_MSAA)
                {
                    if (initPixelFormat == PixelFormat::DEPTH)
                    {
                        texturePixelFormat = DXGI_FORMAT_R32_TYPELESS;
                        shaderViewPixelFormat = DXGI_FORMAT_R32_FLOAT;
                    }
                    else if (initPixelFormat == PixelFormat::DEPTH_STENCIL)
                    {
                        texturePixelFormat = DXGI_FORMAT_R24G8_TYPELESS;
                        shaderViewPixelFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                    }
                }

                width = static_cast<UINT>(levels.front().first.v[0]);
                height = static_cast<UINT>(levels.front().first.v[1]);

                if (!width || !height)
                    throw std::runtime_error("Invalid texture size");

                D3D11_TEXTURE2D_DESC textureDescriptor;
                textureDescriptor.Width = width;
                textureDescriptor.Height = height;
                textureDescriptor.MipLevels = static_cast<UINT>(levels.size());
                textureDescriptor.ArraySize = 1;
                textureDescriptor.Format = texturePixelFormat;
                textureDescriptor.SampleDesc.Count = 1;
                textureDescriptor.SampleDesc.Quality = 0;
                if (flags & Flags::BIND_RENDER_TARGET) textureDescriptor.Usage = D3D11_USAGE_DEFAULT;
                else if (flags & Flags::DYNAMIC) textureDescriptor.Usage = D3D11_USAGE_DYNAMIC;
                else textureDescriptor.Usage = D3D11_USAGE_IMMUTABLE;

                if (flags & Flags::BIND_RENDER_TARGET)
                {
                    if (initPixelFormat == PixelFormat::DEPTH || initPixelFormat == PixelFormat::DEPTH_STENCIL)
                        textureDescriptor.BindFlags = (sampleCount == 1) ? D3D11_BIND_DEPTH_STENCIL : 0;
                    else
                        textureDescriptor.BindFlags = (sampleCount == 1) ? D3D11_BIND_RENDER_TARGET : 0;

                    if (flags & Flags::BIND_SHADER &&
                        !(flags & Flags::BIND_SHADER_MSAA))
                        textureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
                }
                else
                    textureDescriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;

                textureDescriptor.CPUAccessFlags = (flags & Flags::DYNAMIC && !(flags & Flags::BIND_RENDER_TARGET)) ? D3D11_CPU_ACCESS_WRITE : 0;
                textureDescriptor.MiscFlags = 0;

                if (levels.empty() || flags & Flags::BIND_RENDER_TARGET)
                {
                    HRESULT hr;
                    if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&textureDescriptor, nullptr, &texture)))
                        throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 texture");
                }
                else
                {
                    std::vector<D3D11_SUBRESOURCE_DATA> subresourceData(levels.size());

                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        subresourceData[level].pSysMem = levels[level].second.data();
                        subresourceData[level].SysMemPitch = static_cast<UINT>(levels[level].first.v[0] * pixelSize);
                        subresourceData[level].SysMemSlicePitch = 0;
                    }

                    HRESULT hr;
                    if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&textureDescriptor, subresourceData.data(), &texture)))
                        throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 texture");
                }

                if (flags & Flags::BIND_RENDER_TARGET)
                {
                    if (sampleCount > 1)
                    {
                        D3D11_TEXTURE2D_DESC msaaTextureDescriptor;
                        msaaTextureDescriptor.Width = width;
                        msaaTextureDescriptor.Height = height;
                        msaaTextureDescriptor.MipLevels = 1;
                        msaaTextureDescriptor.ArraySize = 1;
                        msaaTextureDescriptor.Format = texturePixelFormat;
                        msaaTextureDescriptor.SampleDesc.Count = sampleCount;
                        msaaTextureDescriptor.SampleDesc.Quality = 0;
                        msaaTextureDescriptor.Usage = D3D11_USAGE_DEFAULT;
                        if (initPixelFormat == PixelFormat::DEPTH || initPixelFormat == PixelFormat::DEPTH_STENCIL)
                            msaaTextureDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                        else
                            msaaTextureDescriptor.BindFlags = D3D11_BIND_RENDER_TARGET;
                        if (flags & Flags::BIND_SHADER_MSAA)
                            msaaTextureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
                        msaaTextureDescriptor.CPUAccessFlags = 0;
                        msaaTextureDescriptor.MiscFlags = 0;

                        HRESULT hr;
                        if (FAILED(hr = renderDevice.getDevice()->CreateTexture2D(&msaaTextureDescriptor, nullptr, &msaaTexture)))
                            throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 texture");

                        if (initPixelFormat == PixelFormat::DEPTH || initPixelFormat == PixelFormat::DEPTH_STENCIL)
                        {
                            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
                            depthStencilViewDesc.Format = pixelFormat;
                            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                            depthStencilViewDesc.Texture2D.MipSlice = 0;
                            depthStencilViewDesc.Flags = 0;

                            if (FAILED(hr = renderDevice.getDevice()->CreateDepthStencilView(msaaTexture, &depthStencilViewDesc, &depthStencilView)))
                                throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 depth stencil view");
                        }
                        else
                        {
                            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                            renderTargetViewDesc.Format = pixelFormat;
                            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                            renderTargetViewDesc.Texture2D.MipSlice = 0;

                            if (FAILED(hr = renderDevice.getDevice()->CreateRenderTargetView(msaaTexture, &renderTargetViewDesc, &renderTargetView)))
                                throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 render target view");
                        }
                    }
                    else
                    {
                        if (initPixelFormat == PixelFormat::DEPTH || initPixelFormat == PixelFormat::DEPTH_STENCIL)
                        {
                            D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
                            depthStencilViewDesc.Format = pixelFormat;
                            depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                            depthStencilViewDesc.Texture2D.MipSlice = 0;
                            depthStencilViewDesc.Flags = 0;

                            HRESULT hr;
                            if (FAILED(hr = renderDevice.getDevice()->CreateDepthStencilView(texture, &depthStencilViewDesc, &depthStencilView)))
                                throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 depth stencil view");
                        }
                        else
                        {
                            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                            renderTargetViewDesc.Format = pixelFormat;
                            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                            renderTargetViewDesc.Texture2D.MipSlice = 0;

                            HRESULT hr;
                            if (FAILED(hr = renderDevice.getDevice()->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTargetView)))
                                throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 render target view");
                        }
                    }

                    if (flags & Flags::BIND_SHADER || flags & Flags::BIND_SHADER_MSAA)
                    {
                        D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
                        resourceViewDesc.Format = shaderViewPixelFormat;
                        resourceViewDesc.ViewDimension = getShaderViewDimension(type, (flags & Flags::BIND_SHADER_MSAA) != 0);
                        resourceViewDesc.Texture2D.MostDetailedMip = 0;
                        resourceViewDesc.Texture2D.MipLevels = 1;

                        HRESULT hr;
                        if (FAILED(hr = renderDevice.getDevice()->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView)))
                            throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 shader resource view");
                    }
                }
                else
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
                    resourceViewDesc.Format = texturePixelFormat;
                    resourceViewDesc.ViewDimension = getShaderViewDimension(type, false);
                    resourceViewDesc.Texture2D.MostDetailedMip = 0;
                    resourceViewDesc.Texture2D.MipLevels = static_cast<UINT>(levels.size());

                    HRESULT hr;
                    if (FAILED(hr = renderDevice.getDevice()->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView)))
                        throw std::system_error(hr, errorCategory, "Failed to create Direct3D 11 shader resource view");
                }

                samplerDescriptor.filter = renderDevice.getTextureFilter();
                samplerDescriptor.addressX = SamplerAddressMode::CLAMP_TO_EDGE;
                samplerDescriptor.addressY = SamplerAddressMode::CLAMP_TO_EDGE;
                samplerDescriptor.addressZ = SamplerAddressMode::CLAMP_TO_EDGE;
                samplerDescriptor.maxAnisotropy = renderDevice.getMaxAnisotropy();

                updateSamplerState();
            }

            Texture::~Texture()
            {
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

            void Texture::setData(const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& levels)
            {
                if (!(flags & Flags::DYNAMIC) || flags & Flags::BIND_RENDER_TARGET)
                    throw std::runtime_error("Texture is not dynamic");

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].second.empty())
                    {
                        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
                        mappedSubresource.pData = nullptr;
                        mappedSubresource.RowPitch = 0;
                        mappedSubresource.DepthPitch = 0;

                        HRESULT hr;
                        if (FAILED(hr = renderDevice.getContext()->Map(texture, static_cast<UINT>(level),
                                                                       (level == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE,
                                                                       0, &mappedSubresource)))
                            throw std::system_error(hr, errorCategory, "Failed to map Direct3D 11 texture");

                        uint8_t* destination = static_cast<uint8_t*>(mappedSubresource.pData);

                        if (mappedSubresource.RowPitch == levels[level].first.v[0] * pixelSize)
                        {
                            std::copy(levels[level].second.begin(),
                                      levels[level].second.end(),
                                      destination);
                        }
                        else
                        {
                            auto source = levels[level].second.begin();
                            auto rowSize = static_cast<uint32_t>(levels[level].first.v[0]) * pixelSize;
                            auto rows = static_cast<UINT>(levels[level].first.v[1]);

                            for (UINT row = 0; row < rows; ++row)
                            {
                                std::copy(source,
                                          source + rowSize,
                                          destination);

                                source += levels[level].first.v[0] * pixelSize;
                                destination += mappedSubresource.RowPitch;
                            }
                        }

                        renderDevice.getContext()->Unmap(texture, static_cast<UINT>(level));
                    }
                }
            }

            void Texture::setFilter(SamplerFilter filter)
            {
                samplerDescriptor.filter = filter;
                updateSamplerState();
            }

            void Texture::setAddressX(SamplerAddressMode addressX)
            {
                samplerDescriptor.addressX = addressX;
                updateSamplerState();
            }

            void Texture::setAddressY(SamplerAddressMode addressY)
            {
                samplerDescriptor.addressY = addressY;
                updateSamplerState();
            }

            void Texture::setAddressZ(SamplerAddressMode addressZ)
            {
                samplerDescriptor.addressZ = addressZ;
                updateSamplerState();
            }

            void Texture::setMaxAnisotropy(uint32_t maxAnisotropy)
            {
                samplerDescriptor.maxAnisotropy = maxAnisotropy;
                updateSamplerState();
            }

            void Texture::resolve()
            {
                if (msaaTexture && texture)
                    renderDevice.getContext()->ResolveSubresource(texture, 0, msaaTexture, 0, pixelFormat);
            }

            void Texture::updateSamplerState()
            {
                if (samplerState) samplerState->Release();
                samplerState = renderDevice.getSamplerState(samplerDescriptor);

                if (!samplerState)
                    throw std::runtime_error("Failed to get D3D11 sampler state");

                samplerState->AddRef();
            }
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif
