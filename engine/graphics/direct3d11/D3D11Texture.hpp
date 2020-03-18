// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11TEXTURE_HPP
#define OUZEL_GRAPHICS_D3D11TEXTURE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <tuple>
#include <utility>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <d3d11.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "graphics/direct3d11/D3D11RenderResource.hpp"
#include "graphics/direct3d11/D3D11Pointer.hpp"
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
                std::uint32_t maxAnisotropy;

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
                        const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels,
                        TextureType type,
                        std::uint32_t initFlags = 0,
                        std::uint32_t initSampleCount = 1,
                        PixelFormat initPixelFormat = PixelFormat::RGBA8UNorm);

                void setData(const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels);
                void setFilter(SamplerFilter filter);
                void setAddressX(SamplerAddressMode addressX);
                void setAddressY(SamplerAddressMode addressY);
                void setAddressZ(SamplerAddressMode addressZ);
                void setMaxAnisotropy(std::uint32_t maxAnisotropy);

                void resolve();

                inline auto getFlags() const noexcept { return flags; }
                inline auto getMipmaps() const noexcept { return mipmaps; }
                inline auto getSampleCount() const noexcept { return sampleCount; }
                inline auto getPixelFormat() const noexcept { return pixelFormat; }

                inline auto& getTexture() const noexcept { return texture; }
                inline auto& getResourceView() const noexcept { return resourceView; }
                inline auto getSamplerState() const noexcept { return samplerState; }

                inline auto& getRenderTargetView() const noexcept { return renderTargetView; }
                inline auto& getDepthStencilView() const noexcept { return depthStencilView; }

                inline auto getWidth() const noexcept { return width; }
                inline auto getHeight() const noexcept { return height; }

            private:
                void updateSamplerState();

                std::uint32_t flags = 0;
                std::uint32_t mipmaps = 0;
                std::uint32_t sampleCount = 1;
                DXGI_FORMAT pixelFormat = DXGI_FORMAT_UNKNOWN;
                std::uint32_t pixelSize = 0;
                SamplerStateDesc samplerDescriptor;

                Pointer<ID3D11Texture2D> texture;
				Pointer<ID3D11Texture2D> msaaTexture;
				Pointer<ID3D11ShaderResourceView> resourceView;
				ID3D11SamplerState* samplerState = nullptr;

                UINT width = 0;
                UINT height = 0;

				Pointer<ID3D11RenderTargetView> renderTargetView;
				Pointer<ID3D11DepthStencilView> depthStencilView;
            };
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_D3D11TEXTURE_HPP
