// Ouzel by Elviss Strazdins

#include <memory>
#include <stdexcept>
#include "ImageLoader.hpp"
#include "Bundle.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Image.hpp"
#include "../graphics/Texture.hpp"

#if defined(_MSC_VER)
#  pragma warning( push )
#  pragma warning( disable : 4100 )
#  pragma warning( disable : 4505 )
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  if defined(__clang__)
#    pragma GCC diagnostic ignored "-Wcomma"
#    pragma GCC diagnostic ignored "-Wmissing-prototypes"
#  endif
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

namespace ouzel::assets
{
    ImageLoader::ImageLoader(Cache& initCache):
        Loader(initCache, Type::image)
    {
    }

    bool ImageLoader::loadAsset(Bundle& bundle,
                                const std::string& name,
                                const std::vector<std::byte>& data,
                                bool mipmaps)
    {
        int width;
        int height;
        int comp;

        using ImageFreeFunction = void(*)(void*);
        std::unique_ptr<stbi_uc, ImageFreeFunction> tempData{
            stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()),
                                  static_cast<int>(data.size()),
                                  &width, &height,
                                  &comp, STBI_default),
            stbi_image_free
        };

        if (!tempData)
            throw std::runtime_error("Failed to load texture, reason: " + std::string(stbi_failure_reason()));

        graphics::PixelFormat pixelFormat;
        std::vector<std::uint8_t> imageData;

        switch (comp)
        {
            case STBI_grey:
            {
                pixelFormat = graphics::PixelFormat::rgba8UnsignedNorm;

                imageData.resize(static_cast<std::size_t>(width * height * 4));

                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        const auto sourceOffset = static_cast<std::size_t>(y * width + x);
                        const auto destinationOffset = static_cast<std::size_t>((y * width + x) * 4);
                        imageData[destinationOffset + 0] = tempData.get()[sourceOffset];
                        imageData[destinationOffset + 1] = tempData.get()[sourceOffset];
                        imageData[destinationOffset + 2] = tempData.get()[sourceOffset];
                        imageData[destinationOffset + 3] = 255;
                    }
                }
                break;
            }
            case STBI_grey_alpha:
            {
                pixelFormat = graphics::PixelFormat::rgba8UnsignedNorm;

                imageData.resize(static_cast<std::size_t>(width * height * 4));

                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        const auto sourceOffset = static_cast<std::size_t>((y * width + x) * 2);
                        const auto destinationOffset = static_cast<std::size_t>((y * width + x) * 4);
                        imageData[destinationOffset + 0] = tempData.get()[sourceOffset + 0];
                        imageData[destinationOffset + 1] = tempData.get()[sourceOffset + 0];
                        imageData[destinationOffset + 2] = tempData.get()[sourceOffset + 0];
                        imageData[destinationOffset + 3] = tempData.get()[sourceOffset + 1];
                    }
                }
                break;
            }
            case STBI_rgb:
            {
                pixelFormat = graphics::PixelFormat::rgba8UnsignedNorm;

                imageData.resize(static_cast<std::size_t>(width * height * 4));

                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        const auto sourceOffset = static_cast<std::size_t>((y * width + x) * 3);
                        const auto destinationOffset = static_cast<std::size_t>((y * width + x) * 4);
                        imageData[destinationOffset + 0] = tempData.get()[sourceOffset + 0];
                        imageData[destinationOffset + 1] = tempData.get()[sourceOffset + 1];
                        imageData[destinationOffset + 2] = tempData.get()[sourceOffset + 2];
                        imageData[destinationOffset + 3] = 255;
                    }
                }
                break;
            }
            case STBI_rgb_alpha:
            {
                pixelFormat = graphics::PixelFormat::rgba8UnsignedNorm;
                imageData.assign(tempData.get(),
                                 tempData.get() + static_cast<std::size_t>(width * height) * 4);
                break;
            }
            default:
                throw std::runtime_error("Unsupported pixel format");
        }

        graphics::Image image(pixelFormat,
                              Size<std::uint32_t, 2>(static_cast<std::uint32_t>(width),
                                                     static_cast<std::uint32_t>(height)),
                              imageData);

        auto texture = std::make_shared<graphics::Texture>(*engine->getGraphics(),
                                                           image.getData(),
                                                           image.getSize(),
                                                           graphics::Flags::none,
                                                           mipmaps ? 0 : 1,
                                                           image.getPixelFormat());

        bundle.setTexture(name, texture);

        return true;
    }
}
