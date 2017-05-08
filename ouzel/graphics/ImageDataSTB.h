// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "graphics/ImageData.h"

namespace ouzel
{
    namespace graphics
    {
        class ImageDataSTB: public ImageData
        {
        public:
            virtual bool initFromFile(const std::string& newFilename,
                                      PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData,
                                        PixelFormat newPixelFormat = PixelFormat::RGBA8_UNORM);
        };
    } // namespace graphics
} // namespace ouzel
