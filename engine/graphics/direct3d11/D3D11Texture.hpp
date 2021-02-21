// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_D3D11TEXTURE_HPP
#define OUZEL_GRAPHICS_D3D11TEXTURE_HPP

#include "../../core/Setup.h"

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

#include "D3D11RenderResource.hpp"
#include "D3D11Pointer.hpp"
#include "../Flags.hpp"
#include "../PixelFormat.hpp"
#include "../SamplerAddressMode.hpp"
#include "../SamplerFilter.hpp"
#include "../TextureType.hpp"
#include "../../math/Size.hpp"

namespace ouzel::graphics::d3d11
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
                Flags initFlags,
                std::uint32_t initSampleCount,
                PixelFormat initPixelFormat,
                SamplerFilter initFilter,
                std::uint32_t initMaxAnisotropy);

        void setData(const std::vector<std::pair<Size2U, std::vector<std::uint8_t>>>& levels);
        void setFilter(SamplerFilter filter);
        void setAddressX(SamplerAddressMode addressX);
        void setAddressY(SamplerAddressMode addressY);
        void setAddressZ(SamplerAddressMode addressZ);
        void setMaxAnisotropy(std::uint32_t maxAnisotropy);

        void resolve();

        auto getFlags() const noexcept { return flags; }
        auto getMipmaps() const noexcept { return mipmaps; }
        auto getSampleCount() const noexcept { return sampleCount; }
        auto getPixelFormat() const noexcept { return pixelFormat; }

        auto& getTexture() const noexcept { return texture; }
        auto& getResourceView() const noexcept { return resourceView; }
        auto getSamplerState() const noexcept { return samplerState; }

        auto& getRenderTargetView() const noexcept { return renderTargetView; }
        auto& getDepthStencilView() const noexcept { return depthStencilView; }

        auto getWidth() const noexcept { return width; }
        auto getHeight() const noexcept { return height; }

    private:
        void updateSamplerState();

        Flags flags = Flags::none;
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
}
#endif

#endif // OUZEL_GRAPHICS_D3D11TEXTURE_HPP
