// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "utils/Noncopyable.h"
#include "graphics/PixelFormat.h"
#include "math/Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class ImageData
        {
        public:
            ImageData();

            const Size2& getSize() const { return size; }
            const std::vector<uint8_t>& getData() const { return data; }
            PixelFormat getPixelFormat() const { return pixelFormat; }

        protected:
            std::string filename;
            Size2 size;
            std::vector<uint8_t> data;
            PixelFormat pixelFormat = PixelFormat::DEFAULT;
        };
    } // namespace graphics
} // namespace ouzel
