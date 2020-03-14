// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_IMAGEDATA_HPP
#define OUZEL_GRAPHICS_IMAGEDATA_HPP

#include <cstdint>
#include <vector>
#include "graphics/PixelFormat.hpp"
#include "math/Size.hpp"

namespace ouzel
{
    namespace graphics
    {
        class Image final
        {
        public:
            Image() = default;

            Image(PixelFormat initPixelFormat,
                  const Size2U& initSize,
                  const std::vector<std::uint8_t>& initData):
                pixelFormat(initPixelFormat), size(initSize), data(initData)
            {
            }

            inline auto getPixelFormat() const noexcept { return pixelFormat; }
            inline auto& getSize() const noexcept { return size; }
            inline auto& getData() const noexcept { return data; }

        private:
            PixelFormat pixelFormat = PixelFormat::Default;
            Size2U size;
            std::vector<std::uint8_t> data;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_IMAGEDATA_HPP
