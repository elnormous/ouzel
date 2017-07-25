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
            return resource->isDynamic();
        }

        Texture::Filter Texture::getFilter() const
        {
            return resource->getFilter();
        }

        bool Texture::setFilter(Filter newFilter)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newFilter]() {
                textureResource->setFilter(newFilter);
            });

            return true;
        }

        Texture::Address Texture::getAddressX() const
        {
            return resource->getAddressX();
        }

        bool Texture::setAddressX(Address newAddressX)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newAddressX]() {
                textureResource->setAddressX(newAddressX);
            });

            return true;
        }

        Texture::Address Texture::getAddressY() const
        {
            return resource->getAddressY();
        }

        bool Texture::setAddressY(Address newAddressY)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newAddressY]() {
                textureResource->setAddressY(newAddressY);
            });

            return true;
        }

        uint32_t Texture::getMaxAnisotropy() const
        {
            return resource->getMaxAnisotropy();
        }

        bool Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                newMaxAnisotropy]() {
                textureResource->setMaxAnisotropy(newMaxAnisotropy);
            });

            return true;
        }

        uint32_t Texture::getSampleCount() const
        {
            return resource->getSampleCount();
        }

        PixelFormat Texture::getPixelFormat() const
        {
            return resource->getPixelFormat();
        }

        bool Texture::getDepth() const
        {
            return resource->getDepth();
        }

        bool Texture::getClearColorBuffer() const
        {
            return resource->getClearColorBuffer();
        }

        void Texture::setClearColorBuffer(bool clear)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                clear]() {
                textureResource->setClearColorBuffer(clear);
            });
        }

        bool Texture::getClearDepthBuffer() const
        {
            return resource->getClearDepthBuffer();
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                clear]() {
                textureResource->setClearDepthBuffer(clear);
            });
        }

        Color Texture::getClearColor() const
        {
            return resource->getClearColor();
        }

        void Texture::setClearColor(Color color)
        {
            TextureResource* textureResource = resource;

            sharedEngine->getRenderer()->executeOnRenderThread([textureResource,
                                                                color]() {
                textureResource->setClearColor(color);
            });
        }
    } // namespace graphics
} // namespace ouzel
