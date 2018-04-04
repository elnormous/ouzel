// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"

extern uint8_t GAMMA_ENCODE[256];
extern float GAMMA_DECODE[256];

namespace ouzel
{
    namespace graphics
    {
        TextureResource::TextureResource()
        {
        }

        TextureResource::~TextureResource()
        {
        }

        bool TextureResource::init(const Size2& newSize,
                                   uint32_t newFlags,
                                   uint32_t newMipmaps,
                                   uint32_t newSampleCount,
                                   PixelFormat newPixelFormat)
        {
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = newSampleCount;
            pixelFormat = newPixelFormat;

            if (!calculateSizes(newSize))
            {
                return false;
            }

            return true;
        }

        bool TextureResource::init(const std::vector<uint8_t>& newData,
                                   const Size2& newSize,
                                   uint32_t newFlags,
                                   uint32_t newMipmaps,
                                   PixelFormat newPixelFormat)
        {
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            if (!calculateSizes(newSize))
            {
                return false;
            }

            if (!calculateData(newData))
            {
                return false;
            }

            return true;
        }

        bool TextureResource::init(const std::vector<Texture::Level>& newLevels,
                                   const Size2& newSize,
                                   uint32_t newFlags,
                                   PixelFormat newPixelFormat)
        {
            levels = newLevels;
            size = newSize;
            flags = newFlags;
            mipmaps = static_cast<uint32_t>(newLevels.size());
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            return true;
        }

        bool TextureResource::setSize(const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC))
            {
                return false;
            }

            if (newSize.width <= 0.0F || newSize.height <= 0.0F)
            {
                return false;
            }

            if (newSize != size)
            {
                if (!calculateSizes(newSize))
                {
                    return false;
                }
            }

            return true;
        }

        bool TextureResource::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
            {
                return false;
            }

            if (newSize.width <= 0.0F || newSize.height <= 0.0F)
            {
                return false;
            }

            if (newSize != size)
            {
                if (!calculateSizes(newSize))
                {
                    return false;
                }
            }

            if (!calculateData(newData))
            {
                return false;
            }

            return true;
        }

        bool TextureResource::calculateSizes(const Size2& newSize)
        {
            levels.clear();
            size = newSize;

            uint32_t newWidth = static_cast<uint32_t>(newSize.width);
            uint32_t newHeight = static_cast<uint32_t>(newSize.height);

            uint32_t pixelSize = getPixelSize(pixelFormat);
            uint32_t pitch = newWidth * pixelSize;
            uint32_t bufferSize = pitch * newHeight;
            levels.push_back({newSize, pitch, std::vector<uint8_t>(bufferSize)});

            if (!(flags & Texture::RENDER_TARGET) && // don't generate mipmaps for render targets
                (engine->getRenderer()->getDevice()->isNPOTTexturesSupported() || (isPOT(newWidth) && isPOT(newHeight))))
            {
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
                    levels.push_back({mipMapSize, pitch, std::vector<uint8_t>(bufferSize)});
                }
            }

            return true;
        }

        static void imageA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

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
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

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
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

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
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

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

        bool TextureResource::calculateData(const std::vector<uint8_t>& newData)
        {
            levels[0].data = newData;

            for (uint32_t level = 1; level < static_cast<uint32_t>(levels.size()); ++level)
            {
                uint32_t previousWidth = static_cast<uint32_t>(levels[level - 1].size.width);
                uint32_t previousHeight = static_cast<uint32_t>(levels[level - 1].size.height);
                uint32_t previousPitch = static_cast<uint32_t>(levels[level - 1].pitch);

                if (pixelFormat == PixelFormat::RGBA8_UNORM) imageRGBA8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                else if (pixelFormat == PixelFormat::RG8_UNORM) imageRG8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                else if (pixelFormat == PixelFormat::R8_UNORM) imageR8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                else if (pixelFormat == PixelFormat::A8_UNORM) imageA8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
            }

            return true;
        }

        bool TextureResource::setFilter(Texture::Filter newFilter)
        {
            filter = newFilter;

            return true;
        }

        bool TextureResource::setAddressX(Texture::Address newAddressX)
        {
            addressX = newAddressX;

            return true;
        }

        bool TextureResource::setAddressY(Texture::Address newAddressY)
        {
            addressY = newAddressY;

            return true;
        }

        bool TextureResource::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            return true;
        }

        bool TextureResource::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            return true;
        }

        bool TextureResource::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            return true;
        }

        bool TextureResource::setClearColor(Color color)
        {
            clearColor = color;

            return true;
        }

        bool TextureResource::setClearDepth(float clear)
        {
            clearDepth = clear;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
