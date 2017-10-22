// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "graphics/ImageData.hpp"

namespace ouzel
{
    namespace graphics
    {
        class ImageDataSTB: public ImageData
        {
        public:
            bool init(const std::string& filename,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            bool init(const std::vector<uint8_t>& newData,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            bool writeToFile(const std::string& filename);
        };
    } // namespace graphics
} // namespace ouzel
