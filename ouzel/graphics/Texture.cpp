// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Texture.h"
#include "Renderer.h"
#include "Image.h"
#include "core/Engine.h"
#include "utils/Utils.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace graphics
    {
        Texture::Texture()
        {
        }

        Texture::~Texture()
        {
        }

        void Texture::free()
        {
            levels.clear();
            uploadData.levels.clear();
        }

        bool Texture::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget)
        {
            free();

            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = newRenderTarget;
            mipMapsGenerated = false;

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool Texture::initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps)
        {
            free();

            filename = newFilename;

            Image image;
            if (!image.initFromFile(filename))
            {
                return false;
            }

            return initFromBuffer(image.getData(), image.getSize(), newDynamic, newMipmaps);
        }

        bool Texture::initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            free();

            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = false;

            if (!calculateData(newData, newSize))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!dynamic)
            {
                return false;
            }

            if (newSize.width <= 0.0f || newSize.height <= 0.0f)
            {
                return false;
            }

            if (!calculateData(newData, newSize))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        static void imageRgba8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstwidth  = width / 2;
            const uint32_t dstheight = height / 2;

            if (dstwidth == 0 ||  dstheight == 0)
            {
                return;
            }

            for (uint32_t y = 0, ystep = pitch * 2; y < dstheight; ++y, src += ystep)
            {
                const uint8_t* rgba = src;
                for (uint32_t x = 0; x < dstwidth; ++x, rgba += 8, dst += 4)
                {
                    float pixels = 0.0f;

                    float r = 0, g = 0, b = 0, a = 0;

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

                    if (rgba[pitch+3])
                    {
                        r += powf(rgba[pitch+0], 2.2f);
                        g += powf(rgba[pitch+1], 2.2f);
                        b += powf(rgba[pitch+2], 2.2f);
                        pixels += 1.0f;
                    }
                    a += rgba[pitch+3];

                    if (rgba[pitch+7] > 0)
                    {
                        r += powf(rgba[pitch+4], 2.2f);
                        g += powf(rgba[pitch+5], 2.2f);
                        b += powf(rgba[pitch+6], 2.2f);
                        pixels += 1.0f;
                    }
                    a += rgba[pitch+7];

                    if (pixels > 0.0f)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                    }
                    else
                    {
                        r = g = b = 0;
                    }

                    a *= 0.25f;
                    r = powf(r, 1.0f / 2.2f);
                    g = powf(g, 1.0f / 2.2f);
                    b = powf(b, 1.0f / 2.2f);
                    dst[0] = (uint8_t)r;
                    dst[1] = (uint8_t)g;
                    dst[2] = (uint8_t)b;
                    dst[3] = (uint8_t)a;
                }
            }
        }

        bool Texture::calculateData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            levels.clear();
            size = newSize;

            uint32_t newWidth = static_cast<uint32_t>(newSize.width);
            uint32_t newHeight = static_cast<uint32_t>(newSize.height);

            uint32_t pitch = newWidth * 4;
            levels.push_back({ newSize, pitch, newData });

            mipMapsGenerated = mipmaps && (sharedEngine->getRenderer()->isNPOTTexturesSupported() || (isPOT(newWidth) && isPOT(newHeight)));

            if (mipMapsGenerated)
            {
                uint32_t bufferSize = newWidth * newHeight * 4;

                if (newWidth == 1)
                {
                    bufferSize *= 2;
                }
                if (newHeight == 1)
                {
                    bufferSize *= 2;
                }

                std::vector<uint8_t> mipMapData(bufferSize);
                std::copy(newData.begin(), newData.begin() + newWidth * newHeight * 4, mipMapData.begin());

                while (newWidth >= 2 && newHeight >= 2)
                {
                    imageRgba8Downsample2x2(newWidth, newHeight, pitch, mipMapData.data(), mipMapData.data());

                    newWidth >>= 1;
                    newHeight >>= 1;

                    Size2 mipMapSize = Size2(static_cast<float>(newWidth), static_cast<float>(newHeight));
                    pitch = newWidth * 4;

                    levels.push_back({ mipMapSize, pitch, mipMapData });
                }

                if (newWidth > newHeight)
                {
                    for (; newWidth >= 2;)
                    {
                        std::copy(mipMapData.begin(), mipMapData.begin() + newWidth * 4, mipMapData.begin() + newWidth * 4);

                        imageRgba8Downsample2x2(newWidth, 2, pitch, mipMapData.data(), mipMapData.data());

                        newWidth >>= 1;

                        Size2 mipMapSize = Size2(static_cast<float>(newWidth), static_cast<float>(newHeight));
                        pitch = newWidth * 4;

                        levels.push_back({ mipMapSize, pitch, mipMapData });
                    }
                }
                else
                {
                    for (; newHeight >= 2;)
                    {
                        uint32_t* src = reinterpret_cast<uint32_t*>(mipMapData.data());
                        for (int32_t i = static_cast<int32_t>(newHeight) - 1; i >= 0; --i)
                        {
                            src[i * 2] = src[i];
                            src[i * 2 + 1] = src[i];
                        }

                        imageRgba8Downsample2x2(2, newHeight, 8, mipMapData.data(), mipMapData.data());

                        newHeight >>= 1;

                        Size2 mipMapSize = Size2(static_cast<float>(newWidth), static_cast<float>(newHeight));
                        levels.push_back({ mipMapSize, pitch, mipMapData });
                    }
                }
            }

            return true;
        }

        void Texture::update()
        {
            uploadData.size = size;
            uploadData.dynamic = dynamic;
            uploadData.mipmaps = mipMapsGenerated;
            uploadData.dirty = dirty;

            uploadData.renderTarget = renderTarget;
            uploadData.levels = std::move(levels);

            dirty = false;
        }
    } // namespace graphics
} // namespace ouzel
