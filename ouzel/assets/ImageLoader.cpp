// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <memory>
#include <stdexcept>
#include "ImageLoader.hpp"
#include "Bundle.hpp"
#include "core/Engine.hpp"
#include "graphics/Image.hpp"
#include "graphics/Texture.hpp"

#if defined(_MSC_VER)
#  pragma warning( push )
#  pragma warning( disable : 4100 )
#  pragma warning( disable : 4505 )
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcomma"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wmissing-prototypes"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#if defined(_MSC_VER)
#  pragma warning( pop )
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

namespace ouzel
{
    namespace assets
    {
        ImageLoader::ImageLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool ImageLoader::loadAsset(Bundle& bundle,
                                    const std::string& name,
                                    const std::vector<uint8_t>& data,
                                    bool mipmaps)
        {
            int width;
            int height;
            int comp;

            stbi_uc* tempData = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &comp, STBI_default);

            if (!tempData)
                throw std::runtime_error("Failed to load texture, reason: " + std::string(stbi_failure_reason()));

            size_t pixelSize;
            graphics::PixelFormat pixelFormat;
            std::vector<uint8_t> imageData;

            switch (comp)
            {
                case STBI_grey:
                {
                    pixelFormat = graphics::PixelFormat::RGBA8UNorm;
                    pixelSize = 4;

                    imageData.resize(static_cast<size_t>(width * height * 4));

                    for (int y = 0; y < height; ++y)
                    {
                        for (int x = 0; x < width; ++x)
                        {
                            auto sourceOffset = static_cast<size_t>(y * width + x);
                            auto destinationOffset = static_cast<size_t>((y * width + x) * 4);
                            imageData[destinationOffset + 0] = tempData[sourceOffset];
                            imageData[destinationOffset + 1] = tempData[sourceOffset];
                            imageData[destinationOffset + 2] = tempData[sourceOffset];
                            imageData[destinationOffset + 3] = 255;
                        }
                    }
                    stbi_image_free(tempData);
                    break;
                }
                case STBI_grey_alpha:
                {
                    pixelFormat = graphics::PixelFormat::RGBA8UNorm;
                    pixelSize = 4;

                    imageData.resize(static_cast<size_t>(width * height * 4));

                    for (int y = 0; y < height; ++y)
                    {
                        for (int x = 0; x < width; ++x)
                        {
                            auto sourceOffset = static_cast<size_t>((y * width + x) * 2);
                            auto destinationOffset = static_cast<size_t>((y * width + x) * 4);
                            imageData[destinationOffset + 0] = tempData[sourceOffset + 0];
                            imageData[destinationOffset + 1] = tempData[sourceOffset + 0];
                            imageData[destinationOffset + 2] = tempData[sourceOffset + 0];
                            imageData[destinationOffset + 3] = tempData[sourceOffset + 1];
                        }
                    }
                    stbi_image_free(tempData);
                    break;
                }
                case STBI_rgb:
                {
                    pixelFormat = graphics::PixelFormat::RGBA8UNorm;
                    pixelSize = 4;

                    imageData.resize(static_cast<size_t>(width * height * 4));

                    for (int y = 0; y < height; ++y)
                    {
                        for (int x = 0; x < width; ++x)
                        {
                            auto sourceOffset = static_cast<size_t>((y * width + x) * 3);
                            auto destinationOffset = static_cast<size_t>((y * width + x) * 4);
                            imageData[destinationOffset + 0] = tempData[sourceOffset + 0];
                            imageData[destinationOffset + 1] = tempData[sourceOffset + 1];
                            imageData[destinationOffset + 2] = tempData[sourceOffset + 2];
                            imageData[destinationOffset + 3] = 255;
                        }
                    }
                    stbi_image_free(tempData);
                    break;
                }
                case STBI_rgb_alpha:
                {
                    pixelFormat = graphics::PixelFormat::RGBA8UNorm;
                    pixelSize = 4;
                    imageData.assign(tempData,
                                     tempData + static_cast<size_t>(width * height) * pixelSize);
                    stbi_image_free(tempData);
                    break;
                }
                default:
                    stbi_image_free(tempData);
                    throw std::runtime_error("Unsupported pixel format");
            }

            graphics::Image image(pixelFormat,
                                  Size2U(static_cast<uint32_t>(width),
                                         static_cast<uint32_t>(height)),
                                  imageData);

            auto texture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
                                                               image.getData(),
                                                               image.getSize(), 0,
                                                               mipmaps ? 0 : 1,
                                                               image.getPixelFormat());

            bundle.setTexture(name, texture);

            return true;
        }
    } // namespace assets
} // namespace ouzel
