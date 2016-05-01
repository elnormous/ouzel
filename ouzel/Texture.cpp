// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "Engine.h"
#include "Renderer.h"
#include "Image.h"
#include "Utils.h"
#include "stb_image_resize.h"

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

        bool Texture::init(const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;

            return true;
        }

        bool Texture::initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps)
        {
            filename = newFilename;
            dynamic = newDynamic;
            mipmaps = newMipmaps;

            Image image;
            if (!image.initFromFile(filename))
            {
                return false;
            }

            return initFromData(image.getData(), image.getSize(), dynamic, mipmaps);
        }

        bool Texture::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            OUZEL_UNUSED(data);

            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;

            return true;
        }

        bool Texture::upload(const void* data, const Size2& newSize)
        {
            if (!dynamic)
            {
                return false;
            }

            if (size.width <= 0.0f || size.height <= 0.0f)
            {
                return false;
            }

            return true;
        }

        bool Texture::uploadData(const void* data, const Size2& newSize)
        {
            size = newSize;

            mipmapSizes.clear();
            mipmapSizes.push_back(newSize);

            uploadMipmap(0, data);

#ifdef OUZEL_SUPPORTS_OPENGLES
            if (mipmaps && isPOT(width) && isPOT(height))
#else
            if (mipmaps)
#endif
            {
                uint32_t newWidth = static_cast<uint32_t>(newSize.width);
                uint32_t newHeight = static_cast<uint32_t>(newSize.height);

                uint32_t oldMipWidth = newWidth;
                uint32_t oldMipHeight = newHeight;

                uint32_t mipWidth = newWidth >> 1;
                uint32_t mipHeight = newHeight >> 1;
                if (mipWidth < 1) mipWidth = 1;
                if (mipHeight < 1) mipHeight = 1;
                uint32_t mipLevel = 1;

                std::vector<uint8_t> oldMipMapData(newWidth * newHeight * 4);
                memcpy(oldMipMapData.data(), data, newWidth * newHeight * 4);

                std::vector<uint8_t> newMipMapData(mipWidth * mipHeight * 4);

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;

                    stbir_resize_uint8_generic(oldMipMapData.data(), oldMipWidth, oldMipHeight, 0,
                                               newMipMapData.data(), mipWidth, mipHeight, 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    mipmapSizes.push_back(Size2(static_cast<float>(mipWidth), static_cast<float>(mipHeight)));

                    uploadMipmap(mipLevel, newMipMapData.data());
                    
                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;
                    
                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    ++mipLevel;
                    
                    newMipMapData.swap(oldMipMapData);
                }
            }

            return true;
        }

        bool Texture::uploadMipmap(uint32_t level, const void* data)
        {
            OUZEL_UNUSED(data);

            if (level >= mipmapSizes.size())
            {
                return false;
            }

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
                    float r = powf(rgba[0], 2.2f);
                    float g = powf(rgba[1], 2.2f);
                    float b = powf(rgba[2], 2.2f);
                    float a = rgba[3];
                    r += powf(rgba[4], 2.2f);
                    g += powf(rgba[5], 2.2f);
                    b += powf(rgba[6], 2.2f);
                    a += rgba[7];
                    r += powf(rgba[pitch+0], 2.2f);
                    g += powf(rgba[pitch+1], 2.2f);
                    b += powf(rgba[pitch+2], 2.2f);
                    a += rgba[pitch+3];
                    r += powf(rgba[pitch+4], 2.2f);
                    g += powf(rgba[pitch+5], 2.2f);
                    b += powf(rgba[pitch+6], 2.2f);
                    a += rgba[pitch+7];

                    r *= 0.25f;
                    g *= 0.25f;
                    b *= 0.25f;
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
    } // namespace graphics
} // namespace ouzel
