// Ouzel by Elviss Strazdins

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <cstring>
#include <stdexcept>
#include "D3D11Texture.hpp"
#include "D3D11ErrorCategory.hpp"
#include "D3D11MappedSubresource.hpp"
#include "D3D11RenderDevice.hpp"

namespace ouzel::graphics::d3d11
{
    namespace
    {
        constexpr DXGI_FORMAT getPixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::a8UnsignedNorm: return DXGI_FORMAT_A8_UNORM;
                case PixelFormat::r8UnsignedNorm: return DXGI_FORMAT_R8_UNORM;
                case PixelFormat::r8SignedNorm: return DXGI_FORMAT_R8_SNORM;
                case PixelFormat::r8UnsignedInt: return DXGI_FORMAT_R8_UINT;
                case PixelFormat::r8SignedInt: return DXGI_FORMAT_R8_SINT;
                case PixelFormat::r16UnsignedNorm: return DXGI_FORMAT_R16_UNORM;
                case PixelFormat::r16SignedNorm: return DXGI_FORMAT_R16_SNORM;
                case PixelFormat::r16UnsignedInt: return DXGI_FORMAT_R16_UINT;
                case PixelFormat::r16SignedInt: return DXGI_FORMAT_R16_SINT;
                case PixelFormat::r16Float: return DXGI_FORMAT_R16_FLOAT;
                case PixelFormat::r32UnsignedInt: return DXGI_FORMAT_R32_UINT;
                case PixelFormat::r32SignedInt: return DXGI_FORMAT_R32_SINT;
                case PixelFormat::r32Float: return DXGI_FORMAT_R32_FLOAT;
                case PixelFormat::rg8UnsignedNorm: return DXGI_FORMAT_R8G8_UNORM;
                case PixelFormat::rg8SignedNorm: return DXGI_FORMAT_R8G8_SNORM;
                case PixelFormat::rg8UnsignedInt: return DXGI_FORMAT_R8G8_UINT;
                case PixelFormat::rg8SignedInt: return DXGI_FORMAT_R8G8_SINT;
                case PixelFormat::rgba8UnsignedNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
                case PixelFormat::rgba8UnsignedNormSRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                case PixelFormat::rgba8SignedNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
                case PixelFormat::rgba8UnsignedInt: return DXGI_FORMAT_R8G8B8A8_UINT;
                case PixelFormat::rgba8SignedInt: return DXGI_FORMAT_R8G8B8A8_SINT;
                case PixelFormat::rgba16UnsignedNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
                case PixelFormat::rgba16SignedNorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
                case PixelFormat::rgba16UnsignedInt: return DXGI_FORMAT_R16G16B16A16_UINT;
                case PixelFormat::rgba16SignedInt: return DXGI_FORMAT_R16G16B16A16_SINT;
                case PixelFormat::rgba16Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
                case PixelFormat::rgba32UnsignedInt: return DXGI_FORMAT_R32G32B32A32_UINT;
                case PixelFormat::rgba32SignedInt: return DXGI_FORMAT_R32G32B32A32_SINT;
                case PixelFormat::rgba32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case PixelFormat::depth: return DXGI_FORMAT_D32_FLOAT;
                case PixelFormat::depthStencil: return DXGI_FORMAT_D24_UNORM_S8_UINT;
                default: throw std::runtime_error{"Invalid pixel format"};
            }
        }

        constexpr D3D11_SRV_DIMENSION getShaderViewDimension(TextureType type, bool multisample)
        {
            if (multisample)
            {
                switch (type)
                {
                    case TextureType::twoDimensional: return D3D11_SRV_DIMENSION_TEXTURE2DMS;
                    default: throw std::runtime_error{"Invalid multisample texture type"};
                }
            }
            else
            {
                switch (type)
                {
                    case TextureType::oneDimensional: return D3D11_SRV_DIMENSION_TEXTURE1D;
                    case TextureType::twoDimensional: return D3D11_SRV_DIMENSION_TEXTURE2D;
                    case TextureType::threeDimensional: return D3D11_SRV_DIMENSION_TEXTURE3D;
                    case TextureType::cube: return D3D11_SRV_DIMENSION_TEXTURE3D;
                    default: throw std::runtime_error{"Invalid texture type"};
                }
            }
        }

        [[maybe_unused]]
        constexpr D3D11_TEXTURECUBE_FACE getCubeFace(CubeFace face)
        {
            switch (face)
            {
                case CubeFace::positiveX: return D3D11_TEXTURECUBE_FACE_POSITIVE_X;
                case CubeFace::negativeX: return D3D11_TEXTURECUBE_FACE_NEGATIVE_X;
                case CubeFace::positiveY: return D3D11_TEXTURECUBE_FACE_POSITIVE_Y;
                case CubeFace::negativeY: return D3D11_TEXTURECUBE_FACE_NEGATIVE_Y;
                case CubeFace::positiveZ: return D3D11_TEXTURECUBE_FACE_POSITIVE_Z;
                case CubeFace::negativeZ: return D3D11_TEXTURECUBE_FACE_NEGATIVE_Z;
                default: throw std::runtime_error{"Invalid cube face"};
            }
        }
    }

    Texture::Texture(RenderDevice& initRenderDevice,
                     const std::vector<std::pair<math::Size<std::uint32_t, 2>, std::vector<std::uint8_t>>>& levels,
                     TextureType type,
                     Flags initFlags,
                     std::uint32_t initSampleCount,
                     PixelFormat initPixelFormat,
                     SamplerFilter initFilter,
                     std::uint32_t initMaxAnisotropy):
        RenderResource{initRenderDevice},
        flags{initFlags},
        mipmaps{static_cast<std::uint32_t>(levels.size())},
        sampleCount{initSampleCount},
        pixelFormat{d3d11::getPixelFormat(initPixelFormat)},
        pixelSize{getPixelSize(initPixelFormat)}
    {
        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget &&
            (mipmaps == 0 || mipmaps > 1))
            throw std::runtime_error{"Invalid mip map count"};

        if (pixelFormat == DXGI_FORMAT_UNKNOWN)
            throw std::runtime_error{"Invalid pixel format"};

        DXGI_FORMAT texturePixelFormat = pixelFormat;
        DXGI_FORMAT shaderViewPixelFormat = pixelFormat;

        if ((flags & Flags::bindShader) == Flags::bindShader ||
            (flags & Flags::bindShaderMsaa) == Flags::bindShaderMsaa)
        {
            if (initPixelFormat == PixelFormat::depth)
            {
                texturePixelFormat = DXGI_FORMAT_R32_TYPELESS;
                shaderViewPixelFormat = DXGI_FORMAT_R32_FLOAT;
            }
            else if (initPixelFormat == PixelFormat::depthStencil)
            {
                texturePixelFormat = DXGI_FORMAT_R24G8_TYPELESS;
                shaderViewPixelFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            }
        }

        width = static_cast<UINT>(levels.front().first.v[0]);
        height = static_cast<UINT>(levels.front().first.v[1]);

        if (!width || !height)
            throw std::runtime_error{"Invalid texture size"};

        D3D11_TEXTURE2D_DESC textureDescriptor;
        textureDescriptor.Width = width;
        textureDescriptor.Height = height;
        textureDescriptor.MipLevels = static_cast<UINT>(levels.size());
        textureDescriptor.ArraySize = 1;
        textureDescriptor.Format = texturePixelFormat;
        textureDescriptor.SampleDesc.Count = 1;
        textureDescriptor.SampleDesc.Quality = 0;
        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget) textureDescriptor.Usage = D3D11_USAGE_DEFAULT;
        else if ((flags & Flags::dynamic) == Flags::dynamic) textureDescriptor.Usage = D3D11_USAGE_DYNAMIC;
        else textureDescriptor.Usage = D3D11_USAGE_IMMUTABLE;

        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
        {
            if (initPixelFormat == PixelFormat::depth || initPixelFormat == PixelFormat::depthStencil)
                textureDescriptor.BindFlags = (sampleCount == 1) ? D3D11_BIND_DEPTH_STENCIL : 0;
            else
                textureDescriptor.BindFlags = (sampleCount == 1) ? D3D11_BIND_RENDER_TARGET : 0;

            if ((flags & Flags::bindShader) == Flags::bindShader &&
                (flags & Flags::bindShaderMsaa) != Flags::bindShaderMsaa)
                textureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        }
        else
            textureDescriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        textureDescriptor.CPUAccessFlags = ((flags & Flags::dynamic) == Flags::dynamic && (flags & Flags::bindRenderTarget) != Flags::bindRenderTarget) ? D3D11_CPU_ACCESS_WRITE : 0;
        textureDescriptor.MiscFlags = 0;

        if (levels.empty() ||
            (flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
        {
            ID3D11Texture2D* newTexture;
            if (const auto hr = renderDevice.getDevice()->CreateTexture2D(&textureDescriptor, nullptr, &newTexture); FAILED(hr))
                throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 texture"};

            texture = newTexture;
        }
        else
        {
            std::vector<D3D11_SUBRESOURCE_DATA> subresourceData(levels.size());

            for (std::size_t level = 0; level < levels.size(); ++level)
            {
                subresourceData[level].pSysMem = levels[level].second.data();
                subresourceData[level].SysMemPitch = static_cast<UINT>(levels[level].first.v[0] * pixelSize);
                subresourceData[level].SysMemSlicePitch = 0;
            }

            ID3D11Texture2D* newTexture;
            if (const auto hr = renderDevice.getDevice()->CreateTexture2D(&textureDescriptor, subresourceData.data(), &newTexture); FAILED(hr))
                throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 texture"};

            texture = newTexture;
        }

        if ((flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
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
                if (initPixelFormat == PixelFormat::depth || initPixelFormat == PixelFormat::depthStencil)
                    msaaTextureDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
                else
                    msaaTextureDescriptor.BindFlags = D3D11_BIND_RENDER_TARGET;
                if ((flags & Flags::bindShaderMsaa) == Flags::bindShaderMsaa)
                    msaaTextureDescriptor.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
                msaaTextureDescriptor.CPUAccessFlags = 0;
                msaaTextureDescriptor.MiscFlags = 0;

                ID3D11Texture2D* newMsaaTexture;
                if (const auto hr = renderDevice.getDevice()->CreateTexture2D(&msaaTextureDescriptor, nullptr, &newMsaaTexture); FAILED(hr))
                    throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 texture"};

                msaaTexture = newMsaaTexture;

                if (initPixelFormat == PixelFormat::depth || initPixelFormat == PixelFormat::depthStencil)
                {
                    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
                    depthStencilViewDesc.Format = pixelFormat;
                    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
                    depthStencilViewDesc.Texture2D.MipSlice = 0;
                    depthStencilViewDesc.Flags = 0;

                    ID3D11DepthStencilView* newDepthStenciView;

                    if (const auto hr = renderDevice.getDevice()->CreateDepthStencilView(msaaTexture.get(), &depthStencilViewDesc, &newDepthStenciView); FAILED(hr))
                        throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 depth stencil view"};

                    depthStencilView = newDepthStenciView;
                }
                else
                {
                    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                    renderTargetViewDesc.Format = pixelFormat;
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                    renderTargetViewDesc.Texture2D.MipSlice = 0;

                    ID3D11RenderTargetView* newRenderTargetView;

                    if (const auto hr = renderDevice.getDevice()->CreateRenderTargetView(msaaTexture.get(), &renderTargetViewDesc, &newRenderTargetView); FAILED(hr))
                        throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 render target view"};

                    renderTargetView = newRenderTargetView;
                }
            }
            else
            {
                if (initPixelFormat == PixelFormat::depth || initPixelFormat == PixelFormat::depthStencil)
                {
                    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
                    depthStencilViewDesc.Format = pixelFormat;
                    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                    depthStencilViewDesc.Texture2D.MipSlice = 0;
                    depthStencilViewDesc.Flags = 0;

                    ID3D11DepthStencilView* newDepthStenciView;
                    if (const auto hr = renderDevice.getDevice()->CreateDepthStencilView(texture.get(), &depthStencilViewDesc, &newDepthStenciView); FAILED(hr))
                        throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 depth stencil view"};

                    depthStencilView = newDepthStenciView;
                }
                else
                {
                    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                    renderTargetViewDesc.Format = pixelFormat;
                    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                    renderTargetViewDesc.Texture2D.MipSlice = 0;

                    ID3D11RenderTargetView* newRenderTargetView;
                    if (const auto hr = renderDevice.getDevice()->CreateRenderTargetView(texture.get(), &renderTargetViewDesc, &newRenderTargetView); FAILED(hr))
                        throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 render target view"};

                    renderTargetView = newRenderTargetView;
                }
            }

            if ((flags & Flags::bindShader) == Flags::bindShader ||
                (flags & Flags::bindShaderMsaa) == Flags::bindShaderMsaa)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
                resourceViewDesc.Format = shaderViewPixelFormat;
                resourceViewDesc.ViewDimension = getShaderViewDimension(type, (flags & Flags::bindShaderMsaa) == Flags::bindShaderMsaa);
                resourceViewDesc.Texture2D.MostDetailedMip = 0;
                resourceViewDesc.Texture2D.MipLevels = 1;

                ID3D11ShaderResourceView* newResourceView;
                if (const auto hr = renderDevice.getDevice()->CreateShaderResourceView(texture.get(), &resourceViewDesc, &newResourceView); FAILED(hr))
                    throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 shader resource view"};

                resourceView = newResourceView;
            }
        }
        else
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
            resourceViewDesc.Format = texturePixelFormat;
            resourceViewDesc.ViewDimension = getShaderViewDimension(type, false);
            resourceViewDesc.Texture2D.MostDetailedMip = 0;
            resourceViewDesc.Texture2D.MipLevels = static_cast<UINT>(levels.size());

            ID3D11ShaderResourceView* newResourceView;
            if (const auto hr = renderDevice.getDevice()->CreateShaderResourceView(texture.get(), &resourceViewDesc, &newResourceView); FAILED(hr))
                throw std::system_error{hr, getErrorCategory(), "Failed to create Direct3D 11 shader resource view"};

            resourceView = newResourceView;
        }

        samplerDescriptor.filter = initFilter;
        samplerDescriptor.addressX = SamplerAddressMode::clampToEdge;
        samplerDescriptor.addressY = SamplerAddressMode::clampToEdge;
        samplerDescriptor.addressZ = SamplerAddressMode::clampToEdge;
        samplerDescriptor.maxAnisotropy = initMaxAnisotropy;

        updateSamplerState();
    }

    void Texture::setData(const std::vector<std::pair<math::Size<std::uint32_t, 2>, std::vector<std::uint8_t>>>& levels)
    {
        if ((flags & Flags::dynamic) != Flags::dynamic ||
            (flags & Flags::bindRenderTarget) == Flags::bindRenderTarget)
            throw std::runtime_error{"Texture is not dynamic"};

        for (std::size_t level = 0; level < levels.size(); ++level)
        {
            if (!levels[level].second.empty())
            {
                MappedSubresource mapped{renderDevice.getContext()};
                const auto mappedSubresource = mapped.map(texture.get(), level,
                                                          (level == 0) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE);

                if (mappedSubresource.RowPitch == levels[level].first.v[0] * pixelSize)
                {
                    std::memcpy(mappedSubresource.pData,
                                levels[level].second.data(),
                                levels[level].second.size());
                }
                else
                {
                    const auto rowSize = levels[level].first.v[0] * pixelSize;
                    const auto rows = static_cast<UINT>(levels[level].first.v[1]);
                    auto source = levels[level].second.data();
                    auto destination = static_cast<BYTE*>(mappedSubresource.pData);

                    for (UINT row = 0; row < rows; ++row)
                    {
                        std::memcpy(destination,
                                    source,
                                    rowSize);

                        source += rowSize;
                        destination += mappedSubresource.RowPitch;
                    }
                }
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

    void Texture::setMaxAnisotropy(std::uint32_t maxAnisotropy)
    {
        samplerDescriptor.maxAnisotropy = maxAnisotropy;
        updateSamplerState();
    }

    void Texture::resolve()
    {
        if (msaaTexture && texture)
            renderDevice.getContext()->ResolveSubresource(texture.get(), 0, msaaTexture.get(), 0, pixelFormat);
    }

    void Texture::updateSamplerState()
    {
        samplerState = renderDevice.getSamplerState(samplerDescriptor);

        if (!samplerState)
            throw std::runtime_error{"Failed to get D3D11 sampler state"};
    }
}

#endif
