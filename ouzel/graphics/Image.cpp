// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Image.h"
#include "utils/Log.h"
#include "core/Application.h"
#include "files/FileSystem.h"
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
        }

        bool Image::initFromFile(const std::string& newFilename)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedApplication->getFileSystem()->loadFile(newFilename, newData))
            {
                return false;
            }

            return initFromBuffer(newData);
        }

        bool Image::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            int width;
            int height;
            int comp;

            stbi_uc* tempData = stbi_load_from_memory(newData.data(), static_cast<int>(newData.size()), &width, &height, &comp, STBI_rgb_alpha);

            if (!tempData)
            {
                Log(Log::Level::ERR) << "Failed to open texture file " << filename << ", reason: " << stbi_failure_reason();
                return false;
            }

            data.assign(tempData, tempData + (width * height * 4));

            stbi_image_free(tempData);

            size.width = static_cast<float>(width);
            size.height = static_cast<float>(height);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
