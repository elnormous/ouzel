// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <functional>
#include <memory>
#include "LoaderImage.hpp"
#include "Cache.hpp"
#include "core/Engine.hpp"
#include "graphics/ImageData.hpp"
#include "graphics/Texture.hpp"
#include "utils/Errors.hpp"
#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace ouzel
{
    namespace assets
    {
        LoaderImage::LoaderImage():
            Loader(TYPE)
        {
        }

        bool LoaderImage::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            int width;
            int height;
            int comp;

            stbi_uc* tempData = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &comp, STBI_default);

            if (!tempData)
                throw ParseError("Failed to load texture, reason: " + std::string(stbi_failure_reason()));

            size_t pixelSize;
            graphics::PixelFormat pixelFormat;
            switch (comp)
            {
                case STBI_grey: pixelFormat = graphics::PixelFormat::R8_UNORM; pixelSize = 1; break;
                case STBI_grey_alpha: pixelFormat = graphics::PixelFormat::RG8_UNORM; pixelSize = 2; break;
                case STBI_rgb_alpha: pixelFormat = graphics::PixelFormat::RGBA8_UNORM; pixelSize = 4; break;
                default:
                    stbi_image_free(tempData);
                    throw ParseError("Unknown pixel size");
            }

            std::vector<uint8_t> imageData(tempData,
                                           tempData + static_cast<size_t>(width * height) * pixelSize);

            stbi_image_free(tempData);

            graphics::ImageData image(pixelFormat,
                                      Size2(static_cast<float>(width), static_cast<float>(height)),
                                      imageData);

            std::shared_ptr<graphics::Texture> texture = std::make_shared<graphics::Texture>(*engine->getRenderer());
            texture->init(image.getData(), image.getSize(), 0, mipmaps ? 0 : 1, image.getPixelFormat());

            cache->setTexture(filename, texture);

            return true;
        }
    } // namespace assets
} // namespace ouzel
