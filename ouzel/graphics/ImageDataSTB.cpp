// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ImageDataSTB.h"
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
        bool ImageDataSTB::initFromFile(const std::string& newFilename,
                                        PixelFormat newPixelFormat)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedApplication->getFileSystem()->readFile(newFilename, newData))
            {
                return false;
            }

            return initFromBuffer(newData, newPixelFormat);
        }

        bool ImageDataSTB::initFromBuffer(const std::vector<uint8_t>& newData,
                                          PixelFormat newPixelFormat)
        {
            int width;
            int height;
            int comp;

            int reqComp;
            switch (newPixelFormat)
            {
                case PixelFormat::R8_UINT: reqComp = STBI_grey; break;
                case PixelFormat::RG8_UINT: reqComp = STBI_grey_alpha; break;
                case PixelFormat::RGB8_UINT: reqComp = STBI_rgb; break;
                case PixelFormat::RGBA8_UINT: reqComp = STBI_rgb_alpha; break;
                default: reqComp = STBI_default;
            }

            stbi_uc* tempData = stbi_load_from_memory(newData.data(), static_cast<int>(newData.size()), &width, &height, &comp, reqComp);

            if (!tempData)
            {
                Log(Log::Level::ERR) << "Failed to open texture file " << filename << ", reason: " << stbi_failure_reason();
                return false;
            }

            size_t pixelSize;
            switch (comp)
            {
                case STBI_grey: pixelFormat = PixelFormat::R8_UINT; pixelSize = 1; break;
                case STBI_grey_alpha: pixelFormat = PixelFormat::RG8_UINT; pixelSize = 2; break;
                case STBI_rgb: pixelFormat = PixelFormat::RGB8_UINT; pixelSize = 3; break;
                case STBI_rgb_alpha: pixelFormat = PixelFormat::RGBA8_UINT; pixelSize = 4; break;
                default:
                    Log(Log::Level::ERR) << "Unknown pixel size";
                    return false;
            }

            data.assign(tempData, tempData + (width * height * pixelSize));

            stbi_image_free(tempData);

            size.v[0] = static_cast<float>(width);
            size.v[1] = static_cast<float>(height);

            return true;
        }
    } // namespace graphics
} // namespace ouzel
