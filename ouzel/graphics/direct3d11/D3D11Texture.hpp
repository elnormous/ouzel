// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11TEXTURE_HPP
#define OUZEL_GRAPHICS_D3D11TEXTURE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <tuple>
#include <d3d11.h>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            class RenderDevice;

            struct SamplerStateDesc final
            {
                ouzel::graphics::Texture::Filter filter;
                ouzel::graphics::Texture::Address addressX;
                ouzel::graphics::Texture::Address addressY;
                ouzel::graphics::Texture::Address addressZ;
                uint32_t maxAnisotropy;

                bool operator<(const SamplerStateDesc& other) const
                {
                    return std::tie(filter, addressX, addressY, addressZ, maxAnisotropy) <
                        std::tie(other.filter, other.addressX, other.addressY, other.addressZ, other.maxAnisotropy);
                }
            };

            class Texture final: public RenderResource
            {
            public:
                Texture(RenderDevice& renderDevice,
                        const std::vector<ouzel::graphics::Texture::Level>& levels,
                        ouzel::graphics::Texture::Dimensions dimensions,
                        uint32_t initFlags = 0,
                        uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
                ~Texture();

                void setData(const std::vector<ouzel::graphics::Texture::Level>& levels);
                void setFilter(ouzel::graphics::Texture::Filter filter);
                void setAddressX(ouzel::graphics::Texture::Address addressX);
                void setAddressY(ouzel::graphics::Texture::Address addressY);
                void setAddressZ(ouzel::graphics::Texture::Address addressZ);
                void setMaxAnisotropy(uint32_t maxAnisotropy);

                void resolve();

                inline uint32_t getFlags() const { return flags; }
                inline uint32_t getMipmaps() const { return mipmaps; }
                inline uint32_t getSampleCount() const { return sampleCount; }
                inline DXGI_FORMAT getPixelFormat() const { return pixelFormat; }

                inline ID3D11Texture2D* getTexture() const { return texture; }
                inline ID3D11ShaderResourceView* getResourceView() const { return resourceView; }
                inline ID3D11SamplerState* getSamplerState() const { return samplerState; }

                inline ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView; }
                inline ID3D11DepthStencilView* getDepthStencilView() const { return depthStencilView; }

                inline UINT getWidth() const { return width; }
                inline UINT getHeight() const { return height; }

            private:
                void updateSamplerState();

                uint32_t flags = 0;
                uint32_t mipmaps = 0;
                uint32_t sampleCount = 1;
                DXGI_FORMAT pixelFormat = DXGI_FORMAT_UNKNOWN;
                uint32_t pixelSize = 0;
                SamplerStateDesc samplerDescriptor;

                ID3D11Texture2D* texture = nullptr;
                ID3D11Texture2D* msaaTexture = nullptr;
                ID3D11ShaderResourceView* resourceView = nullptr;
                ID3D11SamplerState* samplerState = nullptr;

                UINT width = 0;
                UINT height = 0;

                ID3D11RenderTargetView* renderTargetView = nullptr;
                ID3D11DepthStencilView* depthStencilView = nullptr;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11TEXTURE_HPP
