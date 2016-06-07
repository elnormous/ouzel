// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "graphics/Image.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace graphics
    {
        TextureMetal::TextureMetal()
        {

        }

        TextureMetal::~TextureMetal()
        {
            if (texture)
            {
                [texture release];
            }
        }

        void TextureMetal::free()
        {
            Texture::free();

            if (texture)
            {
                [texture release];
                texture = Nil;
            }
        }

        bool TextureMetal::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget)
        {
            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            free();

            return createTexture(static_cast<NSUInteger>(size.width),
                                 static_cast<NSUInteger>(size.height));
        }

        bool TextureMetal::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromData(data, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            free();

            if (!createTexture(static_cast<NSUInteger>(size.width),
                               static_cast<NSUInteger>(size.height)))
            {
                return false;
            }

            ready = true;

            return uploadData(data, newSize);
        }

        bool TextureMetal::uploadMipmap(uint32_t level, const void* data)
        {
            if (!Texture::uploadMipmap(level, data))
            {
                return false;
            }

            NSUInteger newWidth = static_cast<NSUInteger>(mipmapSizes[level].width);
            NSUInteger newHeight = static_cast<NSUInteger>(mipmapSizes[level].height);

            NSUInteger bytesPerRow = newWidth * 4;
            [texture replaceRegion:MTLRegionMake2D(0, 0, newWidth, newHeight) mipmapLevel:level withBytes:data bytesPerRow:bytesPerRow];

            return true;
        }

        bool TextureMetal::uploadData(const void* data, const Size2& newSize)
        {
            if (static_cast<NSUInteger>(newSize.width) != width ||
                static_cast<NSUInteger>(newSize.height) != height)
            {
                free();

                if (!createTexture(static_cast<NSUInteger>(newSize.width),
                                   static_cast<NSUInteger>(newSize.height)))
                {
                    return false;
                }

                ready = true;
            }

            return Texture::uploadData(data, newSize);
        }

        bool TextureMetal::createTexture(NSUInteger newWidth, NSUInteger newHeight)
        {
            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:renderTarget ? rendererMetal->getMetalView().colorPixelFormat : MTLPixelFormatRGBA8Unorm
                                                                                                         width:newWidth
                                                                                                        height:newHeight
                                                                                                     mipmapped:mipmaps ? YES : NO];
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.usage = MTLTextureUsageShaderRead | (renderTarget ? MTLTextureUsageRenderTarget : 0);

            texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

            if (!texture)
            {
                return false;
            }

            width = newWidth;
            height = newHeight;

            return true;
        }
    } // namespace graphics
} // namespace ouzel
