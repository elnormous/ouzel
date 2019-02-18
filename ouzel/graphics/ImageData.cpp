// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "ImageData.hpp"

namespace ouzel
{
    namespace graphics
    {
        ImageData::ImageData()
        {
        }

        ImageData::ImageData(PixelFormat initPixelFormat,
                             const Size2<uint32_t>& initSize,
                             std::vector<uint8_t>& initData):
            pixelFormat(initPixelFormat), size(initSize), data(initData)
        {
        }
    } // namespace graphics
} // namespace ouzel
