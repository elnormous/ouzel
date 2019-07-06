// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11TEXTURE_HPP
#define OUZEL_GRAPHICS_D3D11TEXTURE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <tuple>
#include <utility>
#include <d3d11.h>
#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/PixelFormat.hpp"
#include "graphics/SamplerAddressMode.hpp"
#include "graphics/SamplerFilter.hpp"
#include "graphics/TextureType.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
        {
            class RenderDevice;

            struct SamplerStateDesc final
            {
                SamplerFilter filter;
                SamplerAddressMode addressX;
                SamplerAddressMode addressY;
                SamplerAddressMode addressZ;
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
                Texture(RenderDevice& initRenderDevice,
                        const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& levels,
                        TextureType type,
                        uint32_t initFlags = 0,
                        uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8_UNORM);
                ~Texture();

                void setData(const std::vector<std::pair<Size2U, std::vector<uint8_t>>>& levels);
                void setFilter(SamplerFilter filter);
                void setAddressX(SamplerAddressMode addressX);
                void setAddressY(SamplerAddressMode addressY);
                void setAddressZ(SamplerAddressMode addressZ);
                void setMaxAnisotropy(uint32_t maxAnisotropy);

                void resolve();

                inline auto getFlags() const noexcept { return flags; }
                inline auto getMipmaps() const noexcept { return mipmaps; }
                inline auto getSampleCount() const noexcept { return sampleCount; }
                inline auto getPixelFormat() const noexcept { return pixelFormat; }

                inline auto getTexture() const noexcept { return texture; }
                inline auto getResourceView() const noexcept { return resourceView; }
                inline auto getSamplerState() const noexcept { return samplerState; }

                inline auto getRenderTargetView() const noexcept { return renderTargetView; }
                inline auto getDepthStencilView() const noexcept { return depthStencilView; }

                inline auto getWidth() const noexcept { return width; }
                inline auto getHeight() const noexcept { return height; }

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
