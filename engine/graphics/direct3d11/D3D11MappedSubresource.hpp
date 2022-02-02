// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_D3D11MAPPEDSUBRESOURCE_HPP
#define OUZEL_GRAPHICS_D3D11MAPPEDSUBRESOURCE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <system_error>
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

namespace ouzel::graphics::d3d11
{
    class MappedSubresource final
    {
    public:
        MappedSubresource(ID3D11DeviceContext* context) noexcept:
            deviceContext{context}
        {
        }

        ~MappedSubresource()
        {
            if (mappedResource) deviceContext->Unmap(mappedResource, 0);
        }

        MappedSubresource(const MappedSubresource&) = delete;
        MappedSubresource& operator=(const MappedSubresource&) = delete;

        D3D11_MAPPED_SUBRESOURCE map(ID3D11Resource* resource, std::size_t i, D3D11_MAP mapType)
        {
            if (mappedResource) unmap();

            D3D11_MAPPED_SUBRESOURCE result;
            if (const auto hr = deviceContext->Map(resource, static_cast<UINT>(i), mapType, 0, &result); FAILED(hr))
                throw std::system_error{hr, errorCategory, "Failed to map Direct3D 11 resource"};

            mappedResource = resource;
            index = i;

            return result;
        }

        void unmap()
        {
            deviceContext->Unmap(mappedResource, static_cast<UINT>(index));
            mappedResource = nullptr;
        }

    private:
        ID3D11DeviceContext* deviceContext = nullptr;
        ID3D11Resource* mappedResource = nullptr;
        std::size_t index = 0;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11MAPPEDSUBRESOURCE_HPP