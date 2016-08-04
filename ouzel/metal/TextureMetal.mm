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
        TextureMetal::TextureMetal():
            dirty(true)
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
            std::lock_guard<std::mutex> lock(dataMutex);

            Texture::free();

            if (texture)
            {
                [texture release];
                texture = Nil;
            }
        }

        bool TextureMetal::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            data.clear();

            return true;
        }

        bool TextureMetal::initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            data.clear();

            ready = true;

            return uploadData(newData, newSize);
        }

        bool TextureMetal::upload(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::upload(newData, newSize))
            {
                return false;
            }

            return true;
        }

        bool TextureMetal::uploadMipmap(uint32_t level, const std::vector<uint8_t>& newData)
        {
            if (!Texture::uploadMipmap(level, newData))
            {
                return false;
            }

            if (data.size() < level + 1) data.resize(level + 1);
            data[level] = newData;

            dirty = true;

            return true;
        }

        bool TextureMetal::uploadData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!Texture::uploadData(newData, newSize))
            {
                return false;
            }

            return true;
        }

        bool TextureMetal::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (!texture ||
                    static_cast<NSUInteger>(size.width) != width ||
                    static_cast<NSUInteger>(size.height) != height)
                {
                    if (texture) [texture release];

                    width = static_cast<NSUInteger>(size.width);
                    height = static_cast<NSUInteger>(size.height);

                    MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:renderTarget ? rendererMetal->getMetalView().colorPixelFormat : MTLPixelFormatRGBA8Unorm
                                                                                                                 width:width
                                                                                                                height:height
                                                                                                             mipmapped:mipmaps ? YES : NO];
                    textureDescriptor.textureType = MTLTextureType2D;
                    textureDescriptor.usage = MTLTextureUsageShaderRead | (renderTarget ? MTLTextureUsageRenderTarget : 0);

                    texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

                    if (!texture)
                    {
                        return false;
                    }
                }

                if (!data.empty())
                {
                    for (size_t level = 0; level < data.size(); ++level)
                    {
                        NSUInteger mipWidth = static_cast<NSUInteger>(mipmapSizes[level].width);
                        NSUInteger mipHeight = static_cast<NSUInteger>(mipmapSizes[level].height);

                        NSUInteger bytesPerRow = mipWidth * 4;
                        [texture replaceRegion:MTLRegionMake2D(0, 0, mipWidth, mipHeight) mipmapLevel:level withBytes:data[level].data() bytesPerRow:bytesPerRow];
                    }
                }

                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
