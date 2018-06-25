// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "graphics/PixelFormat.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    namespace graphics
    {
        class ImageData
        {
        public:
            ImageData();

            inline const Size2& getSize() const { return size; }
            inline const std::vector<uint8_t>& getData() const { return data; }
            inline PixelFormat getPixelFormat() const { return pixelFormat; }

        protected:
            Size2 size;
            std::vector<uint8_t> data;
            PixelFormat pixelFormat = PixelFormat::DEFAULT;
        };
    } // namespace graphics
} // namespace ouzel
