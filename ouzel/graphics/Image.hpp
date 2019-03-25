// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_IMAGEDATA_HPP
#define OUZEL_GRAPHICS_IMAGEDATA_HPP

#include <cstdint>
#include <vector>
#include "graphics/PixelFormat.hpp"
#include "math/Size.hpp"
#include "utils/Inline.h"

namespace ouzel
{
    namespace graphics
    {
        class Image final
        {
        public:
            Image()
            {
            }

            Image(PixelFormat initPixelFormat,
                  const Size2U& initSize,
                  std::vector<uint8_t>& initData):
                pixelFormat(initPixelFormat), size(initSize), data(initData)
            {
            }

            FORCEINLINE PixelFormat getPixelFormat() const { return pixelFormat; }
            FORCEINLINE const Size2U& getSize() const { return size; }
            FORCEINLINE const std::vector<uint8_t>& getData() const { return data; }

        private:
            PixelFormat pixelFormat = PixelFormat::DEFAULT;
            Size2U size;
            std::vector<uint8_t> data;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_IMAGEDATA_HPP
