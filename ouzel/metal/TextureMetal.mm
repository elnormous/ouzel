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
    namespace video
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
            if (_texture)
            {
                [_texture release];
                _texture = Nil;
            }
        }

        bool TextureMetal::init(const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::init(size, dynamic, mipmaps))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                         width:static_cast<NSUInteger>(size.width)
                                                                                                        height:static_cast<NSUInteger>(size.height)
                                                                                                     mipmapped:mipmaps ? YES : NO];
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.usage = MTLTextureUsageShaderRead;

            _texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

            return true;
        }

        bool TextureMetal::initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::initFromData(data, size, dynamic, mipmaps))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                         width:static_cast<NSUInteger>(size.width)
                                                                                                        height:static_cast<NSUInteger>(size.height)
                                                                                                     mipmapped:mipmaps ? YES : NO];
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.usage = MTLTextureUsageShaderRead;

            _texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

            return uploadData(data,
                              static_cast<NSUInteger>(size.width),
                              static_cast<NSUInteger>(size.height));
        }

        bool TextureMetal::upload(const void* data, const Size2& size)
        {
            if (!Texture::upload(data, size))
            {
                return false;
            }

            return uploadData(data,
                              static_cast<NSUInteger>(size.width),
                              static_cast<NSUInteger>(size.height));
        }

        bool TextureMetal::uploadData(const void* data, NSUInteger width, NSUInteger height)
        {
            NSUInteger bytesPerRow = width * 4;
            [_texture replaceRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0 withBytes:data bytesPerRow:bytesPerRow];

            _mipLevels = 1;

            if (_mipmaps)
            {
                NSUInteger oldMipWidth = width;
                NSUInteger oldMipHeight = height;

                NSUInteger mipWidth = width >> 1;
                NSUInteger mipHeight = height >> 1;
                NSUInteger mipLevel = 1;
                NSUInteger mipBytesPerRow = mipWidth * 4;

                uint8_t* oldMipMapData = new uint8_t[width * height * 4];
                memcpy(oldMipMapData, data, width * height * 4);

                uint8_t* newMipMapData = new uint8_t[mipWidth * mipHeight * 4];

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;

                    stbir_resize_uint8_generic(oldMipMapData, oldMipWidth, oldMipHeight, 0,
                                               newMipMapData, mipWidth, mipHeight, 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    [_texture replaceRegion:MTLRegionMake2D(0, 0, mipWidth, mipHeight) mipmapLevel:mipLevel withBytes:newMipMapData bytesPerRow:mipBytesPerRow];

                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;

                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    mipLevel++;
                    mipBytesPerRow = mipWidth * 4;

                    uint8_t* temp = oldMipMapData;
                    oldMipMapData = newMipMapData;
                    newMipMapData = temp;
                }
                
                delete [] oldMipMapData;
                delete [] newMipMapData;
                
                _mipLevels = mipLevel;
            }

            return true;
        }

    } // namespace video
} // namespace ouzel
