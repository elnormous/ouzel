// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.h"
#include "TextureResource.h"
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
                           bool newDynamic,
                           bool newMipmaps,
                           bool newRenderTarget,
                           uint32_t newSampleCount,
                           bool newDepth,
                           PixelFormat newPixelFormat)
        {
            filename.clear();
            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = newRenderTarget;
            sampleCount = newSampleCount;
            depth = newDepth;
            pixelFormat = newPixelFormat;

            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newSize,
                                                                newDynamic,
                                                                newMipmaps,
                                                                newRenderTarget,
                                                                newSampleCount,
                                                                newDepth,
                                                                newPixelFormat]() {
                textureResource->init(newSize,
                                      newDynamic,
                                      newMipmaps,
                                      newRenderTarget,
                                      newSampleCount,
                                      newDepth,
                                      newPixelFormat);
            });

            return true;
        }

        bool Texture::init(const std::string& newFilename,
                           bool newDynamic,
                           bool newMipmaps,
                           PixelFormat newPixelFormat)
        {
            filename = newFilename;

            ImageDataSTB image;
            if (!image.init(filename, newPixelFormat))
            {
                return false;
            }

            size = image.getSize();
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = false;
            sampleCount = 1;
            depth = false;
            pixelFormat = image.getPixelFormat();

            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newDynamic,
                                                                newMipmaps,
                                                                newPixelFormat,
                                                                image]() {
                textureResource->init(image.getData(),
                                      image.getSize(),
                                      newDynamic,
                                      newMipmaps,
                                      image.getPixelFormat());
            });

            return true;
        }

        bool Texture::init(const std::vector<uint8_t>& newData,
                           const Size2& newSize,
                           bool newDynamic,
                           bool newMipmaps,
                           PixelFormat newPixelFormat)
        {
            filename.clear();
            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = false;
            sampleCount = 1;
            depth = false;
            pixelFormat = newPixelFormat;

            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newData,
                                                                newSize,
                                                                newDynamic,
                                                                newMipmaps,
                                                                newPixelFormat]() {
                textureResource->init(newData,
                                      newSize,
                                      newDynamic,
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

            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newSize]() {
                textureResource->setSize(newSize);
            });

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            size = newSize;

            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newData,
                                                                newSize]() {
                textureResource->setData(newData, newSize);
            });

            return true;
        }

        bool Texture::isDynamic() const
        {
            return dynamic;
        }

        Texture::Filter Texture::getFilter() const
        {
            return filter;
        }

        bool Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            TextureResource* textureResource = resource;

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

            TextureResource* textureResource = resource;

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

            TextureResource* textureResource = resource;

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

            TextureResource* textureResource = resource;

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

        bool Texture::getDepth() const
        {
            return depth;
        }

        bool Texture::getClearColorBuffer() const
        {
            return clearColorBuffer;
        }

        void Texture::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            TextureResource* textureResource = resource;

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

            TextureResource* textureResource = resource;

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

            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                color]() {
                textureResource->setClearColor(color);
            });
        }
    } // namespace graphics
} // namespace ouzel
