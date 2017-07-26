// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "TextureInterface.h"
#include "Renderer.h"
#include "ImageDataSTB.h"
#include "core/Engine.h"

namespace ouzel
{
    namespace graphics
    {
        Texture::Texture()
        {
            resource = sharedEngine->getRenderer()->createTexture();
        }

        Texture::~Texture()
        {
            if (sharedEngine && resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool Texture::init(const Size2& newSize,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           uint32_t newSampleCount,
                           PixelFormat newPixelFormat)
        {
            filename.clear();
            size = newSize;
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = newSampleCount;
            pixelFormat = newPixelFormat;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newSize,
                                                                newFlags,
                                                                newMipmaps,
                                                                newSampleCount,
                                                                newPixelFormat]() {
                textureResource->init(newSize,
                                      newFlags,
                                      newMipmaps,
                                      newSampleCount,
                                      newPixelFormat);
            });

            return true;
        }

        bool Texture::init(const std::string& newFilename,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           PixelFormat newPixelFormat)
        {
            filename = newFilename;

            ImageDataSTB image;
            if (!image.init(filename, newPixelFormat))
            {
                return false;
            }

            size = image.getSize();
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = image.getPixelFormat();

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newFlags,
                                                                newMipmaps,
                                                                newPixelFormat,
                                                                image]() {
                textureResource->init(image.getData(),
                                      image.getSize(),
                                      newFlags,
                                      newMipmaps,
                                      image.getPixelFormat());
            });

            return true;
        }

        bool Texture::init(const std::vector<uint8_t>& newData,
                           const Size2& newSize,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           PixelFormat newPixelFormat)
        {
            filename.clear();
            size = newSize;
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newData,
                                                                newSize,
                                                                newFlags,
                                                                newMipmaps,
                                                                newPixelFormat]() {
                textureResource->init(newData,
                                      newSize,
                                      newFlags,
                                      newMipmaps,
                                      newPixelFormat);
            });

            return true;
        }

        const Size2& Texture::getSize() const
        {
            return size;
        }

        bool Texture::setSize(const Size2& newSize)
        {
            size = newSize;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newSize]() {
                textureResource->setSize(newSize);
            });

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            size = newSize;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newData,
                                                                newSize]() {
                textureResource->setData(newData, newSize);
            });

            return true;
        }

        Texture::Filter Texture::getFilter() const
        {
            return filter;
        }

        bool Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newFilter]() {
                textureResource->setFilter(newFilter);
            });

            return true;
        }

        Texture::Address Texture::getAddressX() const
        {
            return addressX;
        }

        bool Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newAddressX]() {
                textureResource->setAddressX(newAddressX);
            });

            return true;
        }

        Texture::Address Texture::getAddressY() const
        {
            return addressY;
        }

        bool Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newAddressY]() {
                textureResource->setAddressY(newAddressY);
            });

            return true;
        }

        uint32_t Texture::getMaxAnisotropy() const
        {
            return maxAnisotropy;
        }

        bool Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newMaxAnisotropy]() {
                textureResource->setMaxAnisotropy(newMaxAnisotropy);
            });

            return true;
        }

        uint32_t Texture::getSampleCount() const
        {
            return sampleCount;
        }

        PixelFormat Texture::getPixelFormat() const
        {
            return pixelFormat;
        }

        bool Texture::getClearColorBuffer() const
        {
            return clearColorBuffer;
        }

        void Texture::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                clear]() {
                textureResource->setClearColorBuffer(clear);
            });
        }

        bool Texture::getClearDepthBuffer() const
        {
            return clearDepthBuffer;
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                clear]() {
                textureResource->setClearDepthBuffer(clear);
            });
        }

        Color Texture::getClearColor() const
        {
            return clearColor;
        }

        void Texture::setClearColor(Color color)
        {
            clearColor = color;

            TextureInterface* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                color]() {
                textureResource->setClearColor(color);
            });
        }
    } // namespace graphics
} // namespace ouzel
