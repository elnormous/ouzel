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

        bool TextureMetal::uploadMipmap(uint32_t level, const Size2& mipMapSize, const std::vector<uint8_t>& newData)
        {
            if (!Texture::uploadMipmap(level, mipMapSize, newData))
            {
                return false;
            }

            if (data.size() < level + 1) data.resize(level + 1);

            data[level].width = static_cast<NSUInteger>(mipMapSize.width);
            data[level].height = static_cast<NSUInteger>(mipMapSize.height);
            data[level].data = newData;

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
                std::vector<Data> localData;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    localData = data;
                }

                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (!texture ||
                    static_cast<NSUInteger>(size.width) != width ||
                    static_cast<NSUInteger>(size.height) != height)
                {
                    if (texture) [texture release];

                    width = static_cast<NSUInteger>(size.width);
                    height = static_cast<NSUInteger>(size.height);

                    if (width > 0 && height > 0)
                    {
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
                }

                if (texture && !localData.empty())
                {
                    for (size_t level = 0; level < data.size(); ++level)
                    {
                        NSUInteger bytesPerRow = localData[level].width * 4;
                        [texture replaceRegion:MTLRegionMake2D(0, 0, localData[level].width, localData[level].height)
                                   mipmapLevel:level withBytes:localData[level].data.data()
                                   bytesPerRow:bytesPerRow];
                    }
                }

                if (texture)
                {
                    ready = true;
                }

                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
