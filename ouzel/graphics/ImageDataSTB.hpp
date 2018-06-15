// Copyright (C) 2018 Elviss Strazdins
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
            void init(const std::string& filename,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            void init(const std::vector<uint8_t>& newData,
                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            void writeToFile(const std::string& filename);
        };
    } // namespace graphics
} // namespace ouzel
