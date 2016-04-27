// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureMetal.h"
#include "Engine.h"
#include "RendererMetal.h"
#include "Image.h"
#include "MathUtils.h"
#include "stb_image_resize.h"

namespace ouzel
{
    namespace graphics
    {
        TextureMetal::TextureMetal()
        {

        }

        TextureMetal::~TextureMetal()
        {
            destroy();
        }

        void TextureMetal::destroy()
        {
            if (texture)
            {
                [texture release];
                texture = Nil;
            }
        }

        bool TextureMetal::init(const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::init(newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            return createTexture(nullptr,
                                 static_cast<NSUInteger>(size.width),
                                 static_cast<NSUInteger>(size.height));
        }

        bool TextureMetal::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromData(data, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            return createTexture(data,
                                 static_cast<NSUInteger>(size.width),
                                 static_cast<NSUInteger>(size.height));
        }

        bool TextureMetal::upload(const void* data, const Size2& newSize)
        {
            if (!Texture::upload(data, newSize))
            {
                return false;
            }

            if (static_cast<NSUInteger>(size.width) != width ||
                static_cast<NSUInteger>(size.height) != height)
            {
                if (texture)
                {
                    [texture release];
                    texture = Nil;
                }

                return createTexture(data,
                                     static_cast<NSUInteger>(size.width),
                                     static_cast<NSUInteger>(size.height));
            }
            else
            {
                return uploadData(data,
                                  static_cast<NSUInteger>(size.width),
                                  static_cast<NSUInteger>(size.height));
            }
        }

        bool TextureMetal::createTexture(const void* data, NSUInteger newWidth, NSUInteger newHeight)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                         width:newWidth
                                                                                                        height:newHeight
                                                                                                     mipmapped:mipmaps ? YES : NO];
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.usage = MTLTextureUsageShaderRead;

            texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

            if (data)
            {
                if (!uploadData(data, newWidth, newHeight))
                {
                    return false;
                }
            }
            else
            {
                std::unique_ptr<uint8_t[]> emptyData(new uint8_t[newWidth * newHeight * 4]);
                memset(emptyData.get(), 0, newWidth * newHeight * 4);
                if (!uploadData(emptyData.get(), newWidth, newHeight))
                {
                    return false;
                }
            }

            width = newWidth;
            height = newHeight;

            return true;
        }

        bool TextureMetal::uploadData(const void* data, NSUInteger newWidth, NSUInteger newHeight)
        {
            NSUInteger bytesPerRow = newWidth * 4;
            [texture replaceRegion:MTLRegionMake2D(0, 0, newWidth, newHeight) mipmapLevel:0 withBytes:data bytesPerRow:bytesPerRow];

            mipLevels = 1;

            if (mipmaps)
            {
                NSUInteger oldMipWidth = newWidth;
                NSUInteger oldMipHeight = newHeight;

                NSUInteger mipWidth = newWidth >> 1;
                NSUInteger mipHeight = newHeight >> 1;
                NSUInteger mipLevel = 1;
                NSUInteger mipBytesPerRow;

                uint8_t* oldMipMapData = new uint8_t[newWidth * newHeight * 4];
                memcpy(oldMipMapData, data, newWidth * newHeight * 4);

                uint8_t* newMipMapData = new uint8_t[mipWidth * mipHeight * 4];

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;
                    mipBytesPerRow = mipWidth * 4;

                    stbir_resize_uint8_generic(oldMipMapData, static_cast<int>(oldMipWidth), static_cast<int>(oldMipHeight), 0,
                                               newMipMapData, static_cast<int>(mipWidth), static_cast<int>(mipHeight), 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    [texture replaceRegion:MTLRegionMake2D(0, 0, mipWidth, mipHeight) mipmapLevel:mipLevel withBytes:newMipMapData bytesPerRow:mipBytesPerRow];

                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;

                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    mipLevel++;

                    uint8_t* temp = oldMipMapData;
                    oldMipMapData = newMipMapData;
                    newMipMapData = temp;
                }
                
                delete [] oldMipMapData;
                delete [] newMipMapData;
                
                mipLevels = mipLevel;
            }

            width = newWidth;
            height = newHeight;

            return true;
        }

    } // namespace graphics
} // namespace ouzel
