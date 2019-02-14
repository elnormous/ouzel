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
        class D3D11RenderDevice;

        struct SamplerStateDesc final
        {
            Texture::Filter filter;
            Texture::Address addressX;
            Texture::Address addressY;
            uint32_t maxAnisotropy;

            bool operator<(const SamplerStateDesc& other) const
            {
                return std::tie(filter, addressX, addressY, maxAnisotropy) < std::tie(other.filter, other.addressX, other.addressY, other.maxAnisotropy);
            }
        };

        class D3D11Texture final: public D3D11RenderResource
        {
        public:
            D3D11Texture(D3D11RenderDevice& renderDeviceD3D11,
                         const std::vector<Texture::Level>& levels,
                         Texture::Dimensions dimensions,
                         uint32_t initFlags = 0,
                         uint32_t initSampleCount = 1,
                         PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
            ~D3D11Texture();

            void setData(const std::vector<Texture::Level>& levels);
            void setFilter(Texture::Filter filter);
            void setAddressX(Texture::Address addressX);
            void setAddressY(Texture::Address addressY);
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
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11TEXTURE_HPP
