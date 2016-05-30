// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Image.h"
#include "Utils.h"
#include "Engine.h"
#include "FileSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        Image::Image()
        {

        }

        Image::~Image()
        {
            if (data)
            {
                stbi_image_free(data);
            }
        }

        bool Image::initFromFile(const std::string& newFilename)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedEngine->getFileSystem()->loadFile(newFilename, newData))
            {
                return false;
            }

            return initFromData(newData);
        }

        bool Image::initFromData(const std::vector<uint8_t>& newData)
        {
            int width;
            int height;
            int comp;

            data = stbi_load_from_memory(newData.data(), static_cast<int>(newData.size()), &width, &height, &comp, STBI_rgb_alpha);

            if (!data)
            {
                log("Failed to open texture file %s, reason: %s", filename.c_str(), stbi_failure_reason());
                return false;
            }

            size.width = static_cast<float>(width);
            size.height = static_cast<float>(height);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
