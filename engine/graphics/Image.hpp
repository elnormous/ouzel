#ifndef FLUXION_GRAPHICS_IMAGEDATA_HPP
#define FLUXION_GRAPHICS_IMAGEDATA_HPP

#include <cstdint>
#include <vector>
#include "PixelFormat.hpp"
#include "../math/Size.hpp"

namespace fluxion::graphics
{
    class Image final
    {
    public:
        Image() = default;

        Image(PixelFormat initPixelFormat,
              const math::Size<std::uint32_t, 2>& initSize,
              const std::vector<std::uint8_t>& initData):
            pixelFormat(initPixelFormat), size(initSize), data(initData)
        {
        }

        auto getPixelFormat() const noexcept { return pixelFormat; }
        auto& getSize() const noexcept { return size; }
        auto& getData() const noexcept { return data; }

    private:
        PixelFormat pixelFormat = PixelFormat::rgba8UnsignedNorm;
        math::Size<std::uint32_t, 2> size;
        std::vector<std::uint8_t> data;
    };
}

#endif // FLUXION_GRAPHICS_IMAGEDATA_HPP
