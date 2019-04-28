// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Texture.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"

static constexpr float GAMMA = 2.2F;

namespace ouzel
{
    namespace graphics
    {
        static void imageA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float a = 0.0F;
                        a += pixel[0];
                        a += pixel[1];
                        a += pixel[pitch + 0];
                        a += pixel[pitch + 1];
                        a /= 4.0F;
                        dst[0] = static_cast<uint8_t>(round(a));
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 1)
                {
                    const uint8_t* pixel = src;

                    float a = 0.0F;
                    a += pixel[0];
                    a += pixel[pitch + 0];
                    a /= 2.0F;
                    dst[0] = static_cast<uint8_t>(round(a));
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float a = 0.0F;
                    a += pixel[0];
                    a += pixel[1];
                    a /= 2.0F;
                    dst[0] = static_cast<uint8_t>(round(a));
                }
            }
        }

        static void imageR8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float r = 0.0F;
                        r += pow(pixel[0] / 255.0F, GAMMA);
                        r += pow(pixel[1] / 255.0F, GAMMA);
                        r += pow(pixel[pitch + 0] / 255.0F, GAMMA);
                        r += pow(pixel[pitch + 1] / 255.0F, GAMMA);
                        r /= 4.0F;
                        dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 1)
                {
                    const uint8_t* pixel = src;

                    float r = 0.0F;
                    r += pow(pixel[0] / 255.0F, GAMMA);
                    r += pow(pixel[pitch + 0] / 255.0F, GAMMA);
                    r /= 2.0F;
                    dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float r = 0.0F;
                    r += pow(pixel[0] / 255.0F, GAMMA);
                    r += pow(pixel[1] / 255.0F, GAMMA);
                    r /= 2.0F;
                    dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                }
            }
        }

        static void imageRG8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                    {
                        float r = 0.0F;
                        float g = 0.0F;

                        r += pow(pixel[0] / 255.0F, GAMMA);
                        g += pow(pixel[1] / 255.0F, GAMMA);

                        r += pow(pixel[2] / 255.0F, GAMMA);
                        g += pow(pixel[3] / 255.0F, GAMMA);

                        r += pow(pixel[pitch + 0] / 255.0F, GAMMA);
                        g += pow(pixel[pitch + 1] / 255.0F, GAMMA);

                        r += pow(pixel[pitch + 2] / 255.0F, GAMMA);
                        g += pow(pixel[pitch + 3] / 255.0F, GAMMA);

                        r /= 4.0F;
                        g /= 4.0F;

                        dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                        dst[1] = static_cast<uint8_t>(round(pow(g, 1.0F / GAMMA) * 255.0F));
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;
                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 2)
                {
                    const uint8_t* pixel = src;
                    float r = 0.0F;
                    float g = 0.0F;

                    r += pow(pixel[0] / 255.0F, GAMMA);
                    g += pow(pixel[1] / 255.0F, GAMMA);

                    r += pow(pixel[pitch + 0] / 255.0F, GAMMA);
                    g += pow(pixel[pitch + 1] / 255.0F, GAMMA);

                    r /= 2.0F;
                    g /= 2.0F;

                    dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                    dst[1] = static_cast<uint8_t>(round(pow(g, 1.0F / GAMMA) * 255.0F));
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                {
                    float r = 0.0F;
                    float g = 0.0F;

                    r += pow(pixel[0] / 255.0F, GAMMA);
                    g += pow(pixel[1] / 255.0F, GAMMA);

                    r += pow(pixel[2] / 255.0F, GAMMA);
                    g += pow(pixel[3] / 255.0F, GAMMA);

                    r /= 2.0F;
                    g /= 2.0F;

                    dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                    dst[1] = static_cast<uint8_t>(round(pow(g, 1.0F / GAMMA) * 255.0F));
                }
            }
        }

        static void imageRGBA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;
                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                    {
                        float pixels = 0.0F;
                        float r = 0.0F;
                        float g = 0.0F;
                        float b = 0.0F;
                        float a = 0.0F;

                        if (pixel[3] > 0)
                        {
                            r += pow(pixel[0] / 255.0F, GAMMA);
                            g += pow(pixel[1] / 255.0F, GAMMA);
                            b += pow(pixel[2] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[3];

                        if (pixel[7] > 0)
                        {
                            r += pow(pixel[4] / 255.0F, GAMMA);
                            g += pow(pixel[5] / 255.0F, GAMMA);
                            b += pow(pixel[6] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[7];

                        if (pixel[pitch + 3] > 0)
                        {
                            r += pow(pixel[pitch + 0] / 255.0F, GAMMA);
                            g += pow(pixel[pitch + 1] / 255.0F, GAMMA);
                            b += pow(pixel[pitch + 2] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 3];

                        if (pixel[pitch + 7] > 0)
                        {
                            r += pow(pixel[pitch + 4] / 255.0F, GAMMA);
                            g += pow(pixel[pitch + 5] / 255.0F, GAMMA);
                            b += pow(pixel[pitch + 6] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 7];

                        if (pixels > 0.0F)
                        {
                            r /= pixels;
                            g /= pixels;
                            b /= pixels;
                            a /= 4.0F;
                            dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                            dst[1] = static_cast<uint8_t>(round(pow(g, 1.0F / GAMMA) * 255.0F));
                            dst[2] = static_cast<uint8_t>(round(pow(b, 1.0F / GAMMA) * 255.0F));
                            dst[3] = static_cast<uint8_t>(round(a));
                        }
                        else
                        {
                            dst[0] = 0;
                            dst[1] = 0;
                            dst[2] = 0;
                            dst[3] = 0;
                        }
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 4)
                {
                    const uint8_t* pixel = src;

                    float pixels = 0.0F;
                    float r = 0.0F;
                    float g = 0.0F;
                    float b = 0.0F;
                    float a = 0.0F;

                    if (pixel[3] > 0)
                    {
                        r += pow(pixel[0] / 255.0F, GAMMA);
                        g += pow(pixel[1] / 255.0F, GAMMA);
                        b += pow(pixel[2] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a = pixel[3];

                    if (pixel[pitch + 3] > 0)
                    {
                        r += pow(pixel[pitch + 0] / 255.0F, GAMMA);
                        g += pow(pixel[pitch + 1] / 255.0F, GAMMA);
                        b += pow(pixel[pitch + 2] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a += pixel[pitch + 3];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a /= 2.0F;
                        dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                        dst[1] = static_cast<uint8_t>(round(pow(g, 1.0F / GAMMA) * 255.0F));
                        dst[2] = static_cast<uint8_t>(round(pow(b, 1.0F / GAMMA) * 255.0F));
                        dst[3] = static_cast<uint8_t>(round(a));
                    }
                    else
                    {
                        dst[0] = 0;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = 0;
                    }
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                {
                    float pixels = 0.0F;
                    float r = 0.0F;
                    float g = 0.0F;
                    float b = 0.0F;
                    float a = 0.0F;

                    if (pixel[3] > 0)
                    {
                        r += pow(pixel[0] / 255.0F, GAMMA);
                        g += pow(pixel[1] / 255.0F, GAMMA);
                        b += pow(pixel[2] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a += pixel[3];

                    if (pixel[7] > 0)
                    {
                        r += pow(pixel[4] / 255.0F, GAMMA);
                        g += pow(pixel[5] / 255.0F, GAMMA);
                        b += pow(pixel[6] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a += pixel[7];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a /= 2.0F;
                        dst[0] = static_cast<uint8_t>(round(pow(r, 1.0F / GAMMA) * 255.0F));
                        dst[1] = static_cast<uint8_t>(round(pow(g, 1.0F / GAMMA) * 255.0F));
                        dst[2] = static_cast<uint8_t>(round(pow(b, 1.0F / GAMMA) * 255.0F));
                        dst[3] = static_cast<uint8_t>(round(a));
                    }
                    else
                    {
                        dst[0] = 0;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = 0;
                    }
                }
            }
        }

        static std::vector<Texture::Level> calculateSizes(const Size2U& size,
                                                          const std::vector<uint8_t>& data,
                                                          uint32_t mipmaps,
                                                          PixelFormat pixelFormat)
        {
            std::vector<Texture::Level> levels;

            uint32_t newWidth = size.v[0];
            uint32_t newHeight = size.v[1];

            uint32_t pixelSize = getPixelSize(pixelFormat);
            uint32_t pitch = newWidth * pixelSize;
            uint32_t bufferSize = pitch * newHeight;
            if (data.empty())
                levels.push_back({size, pitch, std::vector<uint8_t>(bufferSize)});
            else
                levels.push_back({size, pitch, data});

            uint32_t previousWidth = newWidth;
            uint32_t previousHeight = newHeight;
            uint32_t previousPitch = pitch;
            std::vector<uint8_t> previousData = data;

            while ((newWidth > 1 || newHeight > 1) &&
                   (mipmaps == 0 || levels.size() < mipmaps))
            {
                newWidth >>= 1;
                newHeight >>= 1;

                if (newWidth < 1) newWidth = 1;
                if (newHeight < 1) newHeight = 1;

                Size2U mipMapSize = Size2U(newWidth, newHeight);
                pitch = newWidth * pixelSize;
                bufferSize = pitch * newHeight;

                if (data.empty())
                    levels.push_back({mipMapSize, pitch, std::vector<uint8_t>(bufferSize)});
                else
                {
                    std::vector<uint8_t> newData(bufferSize);

                    switch (pixelFormat)
                    {
                        case PixelFormat::RGBA8_UNORM:
                            imageRGBA8Downsample2x2(previousWidth, previousHeight, previousPitch,
                                                    previousData.data(), newData.data());
                            break;

                        case PixelFormat::RG8_UNORM:
                            imageRG8Downsample2x2(previousWidth, previousHeight, previousPitch,
                                                  previousData.data(), newData.data());
                            break;

                        case PixelFormat::R8_UNORM:
                            imageR8Downsample2x2(previousWidth, previousHeight, previousPitch,
                                                 previousData.data(), newData.data());
                            break;

                        case PixelFormat::A8_UNORM:
                            imageA8Downsample2x2(previousWidth, previousHeight, previousPitch,
                                                 previousData.data(), newData.data());
                            break;

                        default:
                            throw std::runtime_error("Invalid pixel format");
                    }

                    levels.push_back({mipMapSize, pitch, newData});

                    previousData = std::move(newData);
                }

                previousWidth = newWidth;
                previousHeight = newHeight;
                previousPitch = pitch;
            }

            return levels;
        }

        Texture::Texture(Renderer& initRenderer):
            resource(initRenderer)
        {
        }

        Texture::Texture(Renderer& initRenderer,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         uint32_t initMipmaps,
                         uint32_t initSampleCount,
                         PixelFormat initPixelFormat):
            resource(initRenderer),
            size(initSize),
            flags(initFlags),
            mipmaps(initMipmaps),
            sampleCount(initSampleCount),
            pixelFormat(initPixelFormat)
        {
            if ((flags & BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
                mipmaps = 1;

            std::vector<Level> levels = calculateSizes(size, std::vector<uint8_t>(), mipmaps, pixelFormat);

            initRenderer.addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource.getId(),
                                                                                    levels,
                                                                                    Dimensions::TWO,
                                                                                    flags,
                                                                                    sampleCount,
                                                                                    pixelFormat)));
        }

        Texture::Texture(Renderer& initRenderer,
                         const std::vector<uint8_t>& initData,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         uint32_t initMipmaps,
                         PixelFormat initPixelFormat):
            resource(initRenderer),
            size(initSize),
            flags(initFlags),
            mipmaps(initMipmaps),
            sampleCount(1),
            pixelFormat(initPixelFormat)
        {
            if ((flags & BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
                mipmaps = 1;

            std::vector<Level> levels = calculateSizes(size, initData, mipmaps, pixelFormat);

            initRenderer.addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource.getId(),
                                                                                    levels,
                                                                                    Dimensions::TWO,
                                                                                    flags,
                                                                                    sampleCount,
                                                                                    pixelFormat)));
        }

        Texture::Texture(Renderer& initRenderer,
                         const std::vector<Level>& initLevels,
                         const Size2U& initSize,
                         uint32_t initFlags,
                         PixelFormat initPixelFormat):
            resource(initRenderer),
            size(initSize),
            flags(initFlags),
            mipmaps(static_cast<uint32_t>(initLevels.size())),
            sampleCount(1),
            pixelFormat(initPixelFormat)
        {
            if ((flags & BIND_RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw std::runtime_error("Invalid mip map count");

            std::vector<Level> levels = initLevels;

            if (!initRenderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPowerOfTwo(size.v[0]) || !isPowerOfTwo(size.v[1])))
            {
                mipmaps = 1;
                levels.resize(1);
            }

            initRenderer.addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource.getId(),
                                                                                    levels,
                                                                                    Dimensions::TWO,
                                                                                    flags,
                                                                                    sampleCount,
                                                                                    pixelFormat)));
        }

        void Texture::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::BIND_RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            std::vector<Level> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureDataCommand(resource.getId(),
                                                                                                      levels)));
        }

        void Texture::setData(const std::vector<uint8_t>& newData, CubeFace face)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::BIND_RENDER_TARGET)
                throw std::runtime_error("Texture is not dynamic");

            std::vector<Level> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureDataCommand(resource.getId(),
                                                                                                      levels,
                                                                                                      face)));
        }

        void Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setAddressZ(Address newAddressZ)
        {
            addressZ = newAddressZ;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }

        void Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            if (resource.getId())
                resource.getRenderer()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource.getId(),
                                                                                                            filter,
                                                                                                            addressX,
                                                                                                            addressY,
                                                                                                            addressZ,
                                                                                                            maxAnisotropy)));
        }
    } // namespace graphics
} // namespace ouzel
