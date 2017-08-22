// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"

// GAMMA = 2.2
static const uint8_t GAMMA_ENCODE[] = {
    0, 21, 28, 34, 39, 43, 46, 50, 53, 56, 59, 61, 64, 66, 68, 70, 72, 74, 76,
    78, 80, 82, 84, 85, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 102, 103, 105,
    106, 107, 109, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 122, 123,
    124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
    139, 140, 141, 142, 143, 144, 144, 145, 146, 147, 148, 149, 150, 151, 151,
    152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164,
    164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175,
    175, 176, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185,
    186, 186, 187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195,
    195, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203, 203, 204,
    205, 205, 206, 206, 207, 207, 208, 209, 209, 210, 210, 211, 212, 212, 213,
    213, 214, 214, 215, 215, 216, 217, 217, 218, 218, 219, 219, 220, 220, 221,
    221, 222, 223, 223, 224, 224, 225, 225, 226, 226, 227, 227, 228, 228, 229,
    229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235, 236, 236,
    237, 237, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244,
    244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 251,
    251, 252, 252, 253, 253, 254, 254, 255, 255
};

static const uint8_t GAMMA_DECODE[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7,
    7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15,
    15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25,
    26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37, 38, 39,
    39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75,
    76, 77, 78, 79, 81, 82, 83, 84, 85, 87, 88, 89, 90, 91, 93, 94, 95, 97, 98,
    99, 100, 102, 103, 105, 106, 107, 109, 110, 111, 113, 114, 116, 117, 119,
    120, 121, 123, 124, 126, 127, 129, 130, 132, 133, 135, 137, 138, 140, 141,
    143, 145, 146, 148, 149, 151, 153, 154, 156, 158, 159, 161, 163, 165, 166,
    168, 170, 172, 173, 175, 177, 179, 181, 182, 184, 186, 188, 190, 192, 194,
    196, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223,
    225, 227, 229, 231, 234, 236, 238, 240, 242, 244, 246, 248, 251, 253, 255
};

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

        bool TextureResource::setSize(const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC))
            {
                return false;
            }

            if (newSize.width <= 0.0f || newSize.height <= 0.0f)
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

            if (newSize.width <= 0.0f || newSize.height <= 0.0f)
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
                (sharedEngine->getRenderer()->isNPOTTexturesSupported() || (isPOT(newWidth) && isPOT(newHeight))))
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

        static const float GAMMA = 2.2f;

        static void imageA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float a = 0.0f;
                        a += pixel[0];
                        a += pixel[1];
                        a += pixel[pitch + 0];
                        a += pixel[pitch + 1];
                        a /= 4.0f;
                        dst[0] = static_cast<uint8_t>(a);
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 1)
                {
                    const uint8_t* pixel = src;

                    float a = 0.0f;
                    a += pixel[0];
                    a += pixel[pitch + 0];
                    a /= 2.0f;
                    dst[0] = static_cast<uint8_t>(a);
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float a = 0.0f;
                    a += pixel[0];
                    a += pixel[1];
                    a /= 2.0f;
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
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float r = 0.0f;
                        r += GAMMA_DECODE[pixel[0]];
                        r += GAMMA_DECODE[pixel[1]];
                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        r += GAMMA_DECODE[pixel[pitch + 1]];
                        r /= 4.0f;
                        r = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                        dst[0] = static_cast<uint8_t>(r);
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 1)
                {
                    const uint8_t* pixel = src;

                    float r = 0.0f;
                    r += GAMMA_DECODE[pixel[0]];
                    r += GAMMA_DECODE[pixel[pitch + 0]];
                    r /= 2.0f;
                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float r = 0.0f;
                    r += GAMMA_DECODE[pixel[0]];
                    r += GAMMA_DECODE[pixel[1]];
                    r /= 2.0f;
                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                }
            }
        }

        static void imageRG8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                    {
                        float r = 0.0f, g = 0.0f;

                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];

                        r += GAMMA_DECODE[pixel[2]];
                        g += GAMMA_DECODE[pixel[3]];

                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        g += GAMMA_DECODE[pixel[pitch + 1]];

                        r += GAMMA_DECODE[pixel[pitch + 2]];
                        g += GAMMA_DECODE[pixel[pitch + 3]];

                        r /= 4.0f;
                        g /= 4.0f;

                        r = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                        g = GAMMA_ENCODE[static_cast<uint8_t>(g)];
                        dst[0] = static_cast<uint8_t>(r);
                        dst[1] = static_cast<uint8_t>(g);
                    }
                }
            }
            else if (dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 2)
                {
                    const uint8_t* pixel = src;
                    float r = 0.0f, g = 0.0f;

                    r += GAMMA_DECODE[pixel[0]];
                    g += GAMMA_DECODE[pixel[1]];

                    r += GAMMA_DECODE[pixel[pitch + 0]];
                    g += GAMMA_DECODE[pixel[pitch + 1]];

                    r /= 2.0f;
                    g /= 2.0f;

                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                    dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(g)];
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 4, dst += 2)
                {
                    float r = 0.0f, g = 0.0f;

                    r += GAMMA_DECODE[pixel[0]];
                    g += GAMMA_DECODE[pixel[1]];

                    r += GAMMA_DECODE[pixel[2]];
                    g += GAMMA_DECODE[pixel[3]];

                    r /= 2.0f;
                    g /= 2.0f;

                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                    dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(g)];
                }
            }
        }

        static void imageRGBA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 && dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                    {
                        float pixels = 0.0f;
                        float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

                        if (pixel[3] > 0)
                        {
                            r += GAMMA_DECODE[pixel[0]];
                            g += GAMMA_DECODE[pixel[1]];
                            b += GAMMA_DECODE[pixel[2]];
                            pixels += 1.0f;
                        }
                        a += pixel[3];

                        if (pixel[7] > 0)
                        {
                            r += GAMMA_DECODE[pixel[4]];
                            g += GAMMA_DECODE[pixel[5]];
                            b += GAMMA_DECODE[pixel[6]];
                            pixels += 1.0f;
                        }
                        a += pixel[7];

                        if (pixel[pitch + 3] > 0)
                        {
                            r += GAMMA_DECODE[pixel[pitch + 0]];
                            g += GAMMA_DECODE[pixel[pitch + 1]];
                            b += GAMMA_DECODE[pixel[pitch + 2]];
                            pixels += 1.0f;
                        }
                        a += pixel[pitch + 3];

                        if (pixel[pitch + 7] > 0)
                        {
                            r += GAMMA_DECODE[pixel[pitch + 4]];
                            g += GAMMA_DECODE[pixel[pitch + 5]];
                            b += GAMMA_DECODE[pixel[pitch + 6]];
                            pixels += 1.0f;
                        }
                        a += pixel[pitch + 7];

                        if (pixels > 0.0f)
                        {
                            r /= pixels;
                            g /= pixels;
                            b /= pixels;
                            a *= 0.25f;
                            dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                            dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(g)];
                            dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(b)];
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
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 4)
                {
                    const uint8_t* pixel = src;

                    float pixels = 0.0f;
                    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

                    if (pixel[3] > 0)
                    {
                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];
                        b += GAMMA_DECODE[pixel[2]];
                        pixels += 1.0f;
                    }
                    a = pixel[3];

                    if (pixel[pitch + 3] > 0)
                    {
                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        g += GAMMA_DECODE[pixel[pitch + 1]];
                        b += GAMMA_DECODE[pixel[pitch + 2]];
                        pixels += 1.0f;
                    }
                    a += pixel[pitch + 3];

                    if (pixels > 0.0f)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5f;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                        dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(g)];
                        dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(b)];
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
                    float pixels = 0.0f;
                    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

                    if (pixel[3] > 0)
                    {
                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];
                        b += GAMMA_DECODE[pixel[2]];
                        pixels += 1.0f;
                    }
                    a += pixel[3];

                    if (pixel[7] > 0)
                    {
                        r += GAMMA_DECODE[pixel[4]];
                        g += GAMMA_DECODE[pixel[5]];
                        b += GAMMA_DECODE[pixel[6]];
                        pixels += 1.0f;
                    }
                    a += pixel[7];

                    if (pixels > 0.0f)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5f;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(r)];
                        dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(g)];
                        dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(b)];
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
