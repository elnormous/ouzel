// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
                         uint32_t initFlags = 0,
                         uint32_t initSampleCount = 1,
                         PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
            ~D3D11Texture();

            void setData(const std::vector<Texture::Level>& levels);
            void setFilter(Texture::Filter filter);
            void setAddressX(Texture::Address addressX);
            void setAddressY(Texture::Address addressY);
            void setMaxAnisotropy(uint32_t maxAnisotropy);
            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

            void resolve();

            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getMipmaps() const { return mipmaps; }

            inline float getClearDepth() const { return clearDepth; }
            inline uint32_t getSampleCount() const { return sampleCount; }

            ID3D11Texture2D* getTexture() const { return texture; }
            ID3D11ShaderResourceView* getResourceView() const { return resourceView; }
            ID3D11SamplerState* getSamplerState() const { return samplerState; }

            ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView; }
            ID3D11Texture2D* getDepthStencilTexture() const { return depthStencilTexture; }
            ID3D11DepthStencilView* getDepthStencilView() const { return depthStencilView; }

            const float* getFrameBufferClearColor() const { return frameBufferClearColor; }

            UINT getWidth() const { return width; }
            UINT getHeight() const { return height; }

            bool getClearFrameBufferView() const { return clearFrameBufferView; }
            bool getClearDepthBufferView() const { return clearDepthBufferView; }

        private:
            void updateSamplerState();

            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            float clearDepth = 1.0F;
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
            ID3D11Texture2D* depthStencilTexture = nullptr;
            ID3D11DepthStencilView* depthStencilView = nullptr;

            FLOAT frameBufferClearColor[4]{0.0F, 0.0F, 0.0F, 0.0F};
            bool clearFrameBufferView = true;
            bool clearDepthBufferView = false;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11TEXTURE_HPP
