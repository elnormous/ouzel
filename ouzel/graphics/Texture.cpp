// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Texture.hpp"
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "utils/Errors.hpp"

extern uint8_t GAMMA_ENCODE[256];
extern float GAMMA_DECODE[256];

namespace ouzel
{
    namespace graphics
    {
        Texture::Texture(Renderer& initRenderer):
            renderer(initRenderer)
        {
            resource = renderer.getDevice()->createTexture();
        }

        Texture::~Texture()
        {
            if (resource) renderer.getDevice()->deleteResource(resource);
        }

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
                        dst[0] = static_cast<uint8_t>(a);
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
                    dst[0] = static_cast<uint8_t>(a);
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
                    dst[0] = static_cast<uint8_t>(a);
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
                        r += GAMMA_DECODE[pixel[0]];
                        r += GAMMA_DECODE[pixel[1]];
                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        r += GAMMA_DECODE[pixel[pitch + 1]];
                        r /= 4.0F;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
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
                    r += GAMMA_DECODE[pixel[0]];
                    r += GAMMA_DECODE[pixel[pitch + 0]];
                    r /= 2.0F;
                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float r = 0.0F;
                    r += GAMMA_DECODE[pixel[0]];
                    r += GAMMA_DECODE[pixel[1]];
                    r /= 2.0F;
                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
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

                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];

                        r += GAMMA_DECODE[pixel[2]];
                        g += GAMMA_DECODE[pixel[3]];

                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        g += GAMMA_DECODE[pixel[pitch + 1]];

                        r += GAMMA_DECODE[pixel[pitch + 2]];
                        g += GAMMA_DECODE[pixel[pitch + 3]];

                        r /= 4.0F;
                        g /= 4.0F;

                        r = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                        g = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                        dst[0] = static_cast<uint8_t>(r);
                        dst[1] = static_cast<uint8_t>(g);
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

                    r += GAMMA_DECODE[pixel[0]];
                    g += GAMMA_DECODE[pixel[1]];

                    r += GAMMA_DECODE[pixel[pitch + 0]];
                    g += GAMMA_DECODE[pixel[pitch + 1]];

                    r /= 2.0F;
                    g /= 2.0F;

                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                    dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                {
                    float r = 0.0F;
                    float g = 0.0F;

                    r += GAMMA_DECODE[pixel[0]];
                    g += GAMMA_DECODE[pixel[1]];

                    r += GAMMA_DECODE[pixel[2]];
                    g += GAMMA_DECODE[pixel[3]];

                    r /= 2.0F;
                    g /= 2.0F;

                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                    dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
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
                            r += GAMMA_DECODE[pixel[0]];
                            g += GAMMA_DECODE[pixel[1]];
                            b += GAMMA_DECODE[pixel[2]];
                            pixels += 1.0F;
                        }
                        a += pixel[3];

                        if (pixel[7] > 0)
                        {
                            r += GAMMA_DECODE[pixel[4]];
                            g += GAMMA_DECODE[pixel[5]];
                            b += GAMMA_DECODE[pixel[6]];
                            pixels += 1.0F;
                        }
                        a += pixel[7];

                        if (pixel[pitch + 3] > 0)
                        {
                            r += GAMMA_DECODE[pixel[pitch + 0]];
                            g += GAMMA_DECODE[pixel[pitch + 1]];
                            b += GAMMA_DECODE[pixel[pitch + 2]];
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 3];

                        if (pixel[pitch + 7] > 0)
                        {
                            r += GAMMA_DECODE[pixel[pitch + 4]];
                            g += GAMMA_DECODE[pixel[pitch + 5]];
                            b += GAMMA_DECODE[pixel[pitch + 6]];
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 7];

                        if (pixels > 0.0F)
                        {
                            r /= pixels;
                            g /= pixels;
                            b /= pixels;
                            a *= 0.25F;
                            dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                            dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                            dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(b))];
                            dst[3] = static_cast<uint8_t>(a);
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
                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];
                        b += GAMMA_DECODE[pixel[2]];
                        pixels += 1.0F;
                    }
                    a = pixel[3];

                    if (pixel[pitch + 3] > 0)
                    {
                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        g += GAMMA_DECODE[pixel[pitch + 1]];
                        b += GAMMA_DECODE[pixel[pitch + 2]];
                        pixels += 1.0F;
                    }
                    a += pixel[pitch + 3];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5F;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                        dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                        dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(b))];
                        dst[3] = static_cast<uint8_t>(a);
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
                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];
                        b += GAMMA_DECODE[pixel[2]];
                        pixels += 1.0F;
                    }
                    a += pixel[3];

                    if (pixel[7] > 0)
                    {
                        r += GAMMA_DECODE[pixel[4]];
                        g += GAMMA_DECODE[pixel[5]];
                        b += GAMMA_DECODE[pixel[6]];
                        pixels += 1.0F;
                    }
                    a += pixel[7];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5F;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                        dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                        dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(b))];
                        dst[3] = static_cast<uint8_t>(a);
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

        static std::vector<Texture::Level> calculateSizes(const Size2& size,
                                                          const std::vector<uint8_t>& data,
                                                          uint32_t mipmaps,
                                                          PixelFormat pixelFormat)
        {
            std::vector<Texture::Level> levels;

            uint32_t newWidth = static_cast<uint32_t>(size.width);
            uint32_t newHeight = static_cast<uint32_t>(size.height);

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

                Size2 mipMapSize = Size2(static_cast<float>(newWidth), static_cast<float>(newHeight));
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
                            throw DataError("Invalid pixel format");
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

        void Texture::init(const Size2& newSize,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           uint32_t newSampleCount,
                           PixelFormat newPixelFormat)
        {
            size = newSize;
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = newSampleCount;
            pixelFormat = newPixelFormat;

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            if (!renderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPOT(static_cast<uint32_t>(size.width)) || isPOT(static_cast<uint32_t>(size.height))))
                mipmaps = 1;

            std::vector<Level> levels = calculateSizes(size, std::vector<uint8_t>(), mipmaps, pixelFormat);

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource,
                                                                                             levels,
                                                                                             flags,
                                                                                             sampleCount,
                                                                                             pixelFormat)));
        }

        void Texture::init(const std::vector<uint8_t>& newData,
                           const Size2& newSize,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           PixelFormat newPixelFormat)
        {
            size = newSize;
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            if (!renderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPOT(static_cast<uint32_t>(size.width)) || isPOT(static_cast<uint32_t>(size.height))))
                mipmaps = 1;

            std::vector<Level> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource,
                                                                                             levels,
                                                                                             flags,
                                                                                             sampleCount,
                                                                                             pixelFormat)));
        }

        void Texture::init(const std::vector<Level>& newLevels,
                           const Size2& newSize,
                           uint32_t newFlags,
                           PixelFormat newPixelFormat)
        {
            size = newSize;
            flags = newFlags;
            mipmaps = static_cast<uint32_t>(newLevels.size());
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            std::vector<Level> levels = newLevels;

            if (!renderer.getDevice()->isNPOTTexturesSupported() &&
                (!isPOT(static_cast<uint32_t>(size.width)) || isPOT(static_cast<uint32_t>(size.height))))
            {
                mipmaps = 1;
                levels.resize(1);
            }

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new InitTextureCommand(resource,
                                                                                             levels,
                                                                                             flags,
                                                                                             sampleCount,
                                                                                             pixelFormat)));
        }

        void Texture::setData(const std::vector<uint8_t>& newData)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw DataError("Texture is not dynamic");

            std::vector<Level> levels = calculateSizes(size, newData, mipmaps, pixelFormat);

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureDataCommand(resource,
                                                                                                levels)));
        }

        void Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setClearColor(Color color)
        {
            clearColor = color;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

        void Texture::setClearDepth(float depth)
        {
            clearDepth = depth;

            renderer.getDevice()->addCommand(std::unique_ptr<Command>(new SetTextureParametersCommand(resource,
                                                                                                      filter,
                                                                                                      addressX,
                                                                                                      addressY,
                                                                                                      maxAnisotropy,
                                                                                                      clearColorBuffer,
                                                                                                      clearDepthBuffer,
                                                                                                      clearColor,
                                                                                                      clearDepth)));
        }

    } // namespace graphics
} // namespace ouzel
