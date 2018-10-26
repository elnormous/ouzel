// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>
#include "graphics/PixelFormat.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class ImageData final
        {
        public:
            ImageData();
            ImageData(PixelFormat initPixelFormat,
                      const Size2& initSize,
                      std::vector<uint8_t>& initData);

            inline PixelFormat getPixelFormat() const { return pixelFormat; }
            inline const Size2& getSize() const { return size; }
            inline const std::vector<uint8_t>& getData() const { return data; }

        private:
            PixelFormat pixelFormat = PixelFormat::DEFAULT;
            Size2 size;
            std::vector<uint8_t> data;
        };
    } // namespace graphics
} // namespace ouzel
