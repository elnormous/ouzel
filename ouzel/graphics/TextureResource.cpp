// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "TextureResource.h"
#include "Renderer.h"
#include "core/Engine.h"
#include "math/MathUtils.h"

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
                                   bool newDynamic,
                                   bool newMipmaps,
                                   bool newRenderTarget,
                                   uint32_t newSampleCount,
                                   bool newDepth,
                                   PixelFormat newPixelFormat)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = newRenderTarget;
            sampleCount = newSampleCount;
            depth = newDepth;
            pixelFormat = newPixelFormat;

            if (!calculateSizes(newSize))
            {
                return false;
            }

            dirty |= DIRTY_DATA | DIRTY_PARAMETERS;

            return true;
        }

        bool TextureResource::initFromBuffer(const std::vector<uint8_t>& newData,
                                             const Size2& newSize,
                                             bool newDynamic,
                                             bool newMipmaps,
                                             PixelFormat newPixelFormat)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = false;
            sampleCount = 1;
            depth = false;
            pixelFormat = newPixelFormat;

            if (!calculateSizes(newSize))
            {
                return false;
            }

            if (!calculateData(newData))
            {
                return false;
            }

            dirty |= DIRTY_DATA | DIRTY_PARAMETERS;

            return true;
        }

        bool TextureResource::setSize(const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (!dynamic)
            {
                return false;
            }

            if (newSize.v[0] <= 0.0f || newSize.v[1] <= 0.0f)
            {
                return false;
            }

            if (!calculateSizes(newSize))
            {
                return false;
            }

            dirty |= DIRTY_DATA;

            return true;
        }

        bool TextureResource::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (!dynamic)
            {
                return false;
            }

            if (newSize.v[0] <= 0.0f || newSize.v[1] <= 0.0f)
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

            dirty |= DIRTY_DATA;

            return true;
        }

        bool TextureResource::calculateSizes(const Size2& newSize)
        {
            levels.clear();
            size = newSize;

            uint32_t newWidth = static_cast<uint32_t>(newSize.v[0]);
            uint32_t newHeight = static_cast<uint32_t>(newSize.v[1]);

            uint32_t pixelSize = getPixelSize(pixelFormat);
            uint32_t pitch = newWidth * pixelSize;
            uint32_t bufferSize = newWidth * pitch;
            levels.push_back({newSize, pitch, std::vector<uint8_t>(bufferSize)});

            if (mipmaps && !renderTarget && (sharedEngine->getRenderer()->isNPOTTexturesSupported() || (isPOT(newWidth) && isPOT(newHeight))))
            {
                while (newWidth > 1 || newHeight > 1)
                {
                    newWidth >>= 1;
                    newHeight >>= 1;

                    if (newWidth < 1) newWidth = 1;
                    if (newHeight < 1) newHeight = 1;

                    Size2 mipMapSize = Size2(static_cast<float>(newWidth), static_cast<float>(newHeight));
                    pitch = newWidth * pixelSize;
                    bufferSize = newWidth * pitch;
                    levels.push_back({mipMapSize, pitch, std::vector<uint8_t>(bufferSize)});
                }
            }

            return true;
        }

        static void imageA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 &&  dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* rgb = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, rgb += 2, dst += 1)
                    {
                        float a = 0.0f;
                        a += rgb[0];
                        a += rgb[1];
                        a += rgb[pitch + 0];
                        a += rgb[pitch + 1];
                        a /= 4.0f;
                        dst[0] = static_cast<uint8_t>(a);
                    }
                }
            }
            else if (dstWidth == 0 && dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* rgb = src;

                    float a = 0.0f;
                    a += rgb[0];
                    a += rgb[pitch];
                    a /= 2.0f;
                    dst[0] = static_cast<uint8_t>(a);
                }
            }
            else if (dstWidth > 0 && dstHeight == 0)
            {
                const uint8_t* rgb = src;
                for (uint32_t x = 0; x < dstWidth; ++x, rgb += 2, dst += 1)
                {
                    float a = 0.0f;
                    a += rgb[0];
                    a += rgb[1];
                    a /= 2.0f;
                    dst[0] = static_cast<uint8_t>(a);
                }
            }
        }

        static void imageR8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 &&  dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* rgb = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, rgb += 2, dst += 1)
                    {
                        float r = 0.0f;
                        r += powf(rgb[0], 2.2f);
                        r += powf(rgb[1], 2.2f);
                        r += powf(rgb[pitch + 0], 2.2f);
                        r += powf(rgb[pitch + 1], 2.2f);
                        r /= 4.0f;
                        r = powf(r, 1.0f / 2.2f);
                        dst[0] = static_cast<uint8_t>(r);
                    }
                }
            }
            else if (dstWidth == 0 && dstHeight > 0)
            {
                // TODO: implement
            }
            else if (dstWidth > 0 && dstHeight == 0)
            {
                // TODO: implement
            }
        }

        static void imageRG8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 &&  dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* rgb = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, rgb += 4, dst += 2)
                    {
                        float r = 0.0f, g = 0.0f;

                        r += powf(rgb[0], 2.2f);
                        g += powf(rgb[1], 2.2f);

                        r += powf(rgb[2], 2.2f);
                        g += powf(rgb[3], 2.2f);

                        r += powf(rgb[pitch + 0], 2.2f);
                        g += powf(rgb[pitch + 1], 2.2f);

                        r += powf(rgb[pitch + 2], 2.2f);
                        g += powf(rgb[pitch + 3], 2.2f);

                        r /= 4.0f;
                        g /= 4.0f;

                        r = powf(r, 1.0f / 2.2f);
                        g = powf(g, 1.0f / 2.2f);
                        dst[0] = static_cast<uint8_t>(r);
                        dst[1] = static_cast<uint8_t>(g);
                    }
                }
            }
            else if (dstWidth == 0 && dstHeight > 0)
            {
                // TODO: implement
            }
            else if (dstWidth > 0 && dstHeight == 0)
            {
                // TODO: implement
            }
        }

        static void imageRGBA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width / 2;
            const uint32_t dstHeight = height / 2;

            if (dstWidth > 0 &&  dstHeight > 0)
            {
                for (uint32_t y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* rgba = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, rgba += 8, dst += 4)
                    {
                        float pixels = 0.0f;
                        float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

                        if (rgba[3] > 0)
                        {
                            r += powf(rgba[0], 2.2f);
                            g += powf(rgba[1], 2.2f);
                            b += powf(rgba[2], 2.2f);
                            pixels += 1.0f;
                        }
                        a = rgba[3];

                        if (rgba[7] > 0)
                        {
                            r += powf(rgba[4], 2.2f);
                            g += powf(rgba[5], 2.2f);
                            b += powf(rgba[6], 2.2f);
                            pixels += 1.0f;
                        }
                        a += rgba[7];

                        if (rgba[pitch + 3] > 0)
                        {
                            r += powf(rgba[pitch + 0], 2.2f);
                            g += powf(rgba[pitch + 1], 2.2f);
                            b += powf(rgba[pitch + 2], 2.2f);
                            pixels += 1.0f;
                        }
                        a += rgba[pitch + 3];

                        if (rgba[pitch + 7] > 0)
                        {
                            r += powf(rgba[pitch + 4], 2.2f);
                            g += powf(rgba[pitch + 5], 2.2f);
                            b += powf(rgba[pitch + 6], 2.2f);
                            pixels += 1.0f;
                        }
                        a += rgba[pitch + 7];

                        if (pixels > 0.0f)
                        {
                            r /= pixels;
                            g /= pixels;
                            b /= pixels;
                        }
                        else
                        {
                            r = g = b = 0.0f;
                        }

                        r = powf(r, 1.0f / 2.2f);
                        g = powf(g, 1.0f / 2.2f);
                        b = powf(b, 1.0f / 2.2f);
                        a *= 0.25f;
                        dst[0] = static_cast<uint8_t>(r);
                        dst[1] = static_cast<uint8_t>(g);
                        dst[2] = static_cast<uint8_t>(b);
                        dst[3] = static_cast<uint8_t>(a);
                    }
                }
            }
            else if (dstWidth == 0 && dstHeight > 0)
            {
                // TODO: implement
            }
            else if (dstWidth > 0 && dstHeight == 0)
            {
                // TODO: implement
            }
        }

        bool TextureResource::calculateData(const std::vector<uint8_t>& newData)
        {
            levels[0].data = newData;

            for (uint32_t level = 1; level < static_cast<uint32_t>(levels.size()); ++level)
            {
                uint32_t previousWidth = static_cast<uint32_t>(levels[level - 1].size.v[0]);
                uint32_t previousHeight = static_cast<uint32_t>(levels[level - 1].size.v[1]);
                uint32_t previousPitch = static_cast<uint32_t>(levels[level - 1].pitch);

                if (previousWidth >= 2 && previousHeight >= 2)
                {
                    if (pixelFormat == PixelFormat::RGBA8_UNORM) imageRGBA8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                    else if (pixelFormat == PixelFormat::RG8_UNORM) imageRG8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                    else if (pixelFormat == PixelFormat::R8_UNORM) imageR8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                    else if (pixelFormat == PixelFormat::A8_UNORM) imageA8Downsample2x2(previousWidth, previousHeight, previousPitch, levels[level - 1].data.data(), levels[level].data.data());
                }
            }

            return true;
        }

        bool TextureResource::setFilter(Texture::Filter newFilter)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            filter = newFilter;
            dirty |= DIRTY_PARAMETERS;

            return true;
        }

        bool TextureResource::setAddressX(Texture::Address newAddressX)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            addressX = newAddressX;
            dirty |= DIRTY_PARAMETERS;

            return true;
        }

        bool TextureResource::setAddressY(Texture::Address newAddressY)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            addressY = newAddressY;
            dirty |= DIRTY_PARAMETERS;

            return true;
        }

        bool TextureResource::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            maxAnisotropy = newMaxAnisotropy;
            dirty |= DIRTY_PARAMETERS;

            return true;
        }

        void TextureResource::setClearColorBuffer(bool clear)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearColorBuffer = clear;
            dirty |= DIRTY_PARAMETERS;
        }

        void TextureResource::setClearDepthBuffer(bool clear)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearDepthBuffer = clear;
            dirty |= DIRTY_PARAMETERS;
        }

        void TextureResource::setClearColor(Color color)
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            clearColor = color;
            dirty |= DIRTY_PARAMETERS;
        }
    } // namespace graphics
} // namespace ouzel
