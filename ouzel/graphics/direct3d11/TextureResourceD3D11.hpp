// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <tuple>
#include <d3d11.h>
#include "graphics/direct3d11/RenderResourceD3D11.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceD3D11;

        struct SamplerStateDesc
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

        class TextureResourceD3D11 final: public RenderResourceD3D11
        {
        public:
            explicit TextureResourceD3D11(RenderDeviceD3D11& renderDeviceD3D11);
            ~TextureResourceD3D11();

            void init(const std::vector<Texture::Level>& levels,
                      uint32_t newFlags = 0,
                      uint32_t newSampleCount = 1,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);

            void setData(const std::vector<Texture::Level>& levels);
            void setFilter(Texture::Filter filter);
            void setAddressX(Texture::Address addressX);
            void setAddressY(Texture::Address addressY);
            void setMaxAnisotropy(uint32_t maxAnisotropy);
            void setClearColorBuffer(bool clear);
            void setClearDepthBuffer(bool clear);
            void setClearColor(Color color);
            void setClearDepth(float newClearDepth);

            inline uint32_t getFlags() const { return flags; }
            inline uint32_t getMipmaps() const { return mipmaps; }

            inline bool getClearColorBuffer() const { return clearColorBuffer; }
            inline bool getClearDepthBuffer() const { return clearDepthBuffer; }
            inline Color getClearColor() const { return clearColor; }
            inline float getClearDepth() const { return clearDepth; }
            inline uint32_t getSampleCount() const { return sampleCount; }
            inline PixelFormat getPixelFormat() const { return pixelFormat; }

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
            void createTexture(const std::vector<Texture::Level>& levels);
            void updateSamplerState();

            uint32_t flags = 0;
            uint32_t mipmaps = 0;
            bool clearColorBuffer = true;
            bool clearDepthBuffer = false;
            Color clearColor;
            float clearDepth = 1.0F;
            uint32_t sampleCount = 1;
            PixelFormat pixelFormat = PixelFormat::RGBA8_UNORM;
            SamplerStateDesc samplerDescriptor;

            ID3D11Texture2D* texture = nullptr;
            ID3D11ShaderResourceView* resourceView = nullptr;
            ID3D11SamplerState* samplerState = nullptr;

            UINT width = 0;
            UINT height = 0;

            ID3D11RenderTargetView* renderTargetView = nullptr;
            ID3D11Texture2D* depthStencilTexture = nullptr;
            ID3D11DepthStencilView* depthStencilView = nullptr;

            FLOAT frameBufferClearColor[4];
            bool clearFrameBufferView = true;
            bool clearDepthBufferView = false;
        };
    } // namespace graphics
} // namespace ouzel

#endif
