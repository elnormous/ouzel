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
            dirty(false)
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

            data.clear();

            if (texture)
            {
                [texture release];
                texture = Nil;
            }
        }

        bool TextureMetal::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool TextureMetal::initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return uploadData(newData, newSize);
        }

        bool TextureMetal::upload(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            data.clear();

            if (!Texture::upload(newData, newSize))
            {
                return false;
            }

            dirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

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
                Size2 localSize;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);
                    localData = data;
                    localSize = size;
                }

                std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

                if (localSize.width > 0 && localSize.height > 0)
                {
                    if (!texture ||
                        static_cast<NSUInteger>(localSize.width) != width ||
                        static_cast<NSUInteger>(localSize.height) != height)
                    {
                        if (texture) [texture release];

                        width = static_cast<NSUInteger>(localSize.width);
                        height = static_cast<NSUInteger>(localSize.height);

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

                    if (!localData.empty())
                    {
                        for (size_t level = 0; level < localData.size(); ++level)
                        {
                            NSUInteger bytesPerRow = localData[level].width * 4;
                            [texture replaceRegion:MTLRegionMake2D(0, 0, localData[level].width, localData[level].height)
                                       mipmapLevel:level withBytes:localData[level].data.data()
                                       bytesPerRow:bytesPerRow];
                        }
                    }
                }

                ready = (texture != Nil);
                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
