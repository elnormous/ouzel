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
            if (resource) sharedEngine->getRenderer()->deleteResource(resource);
        }

        bool Texture::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget, uint32_t newSampleCount, bool newDepth)
        {
            filename.clear();

            if (!resource->init(newSize, newDynamic, newMipmaps, newRenderTarget, newSampleCount, newDepth))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::initFromFile(const std::string& newFilename, bool newDynamic, bool newMipmaps)
        {
            filename = newFilename;

            ImageDataSTB image;
            if (!image.initFromFile(filename))
            {
                return false;
            }

            if (!resource->initFromBuffer(image.getData(), image.getSize(), newDynamic, newMipmaps))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::initFromBuffer(const std::vector<uint8_t>& newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            filename.clear();

            if (!resource->initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        const Size2& Texture::getSize() const
        {
            return resource->getSize();
        }

        bool Texture::setSize(const Size2& newSize)
        {
            if (!resource->setSize(newSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!resource->setData(newData, newSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

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
            if (!resource->setFilter(newFilter))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        Texture::Address Texture::getAddressX() const
        {
            return resource->getAddressX();
        }

        bool Texture::setAddressX(Address newAddressX)
        {
            if (!resource->setAddressX(newAddressX))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        Texture::Address Texture::getAddressY() const
        {
            return resource->getAddressY();
        }

        bool Texture::setAddressY(Address newAddressY)
        {
            if (!resource->setAddressY(newAddressY))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        uint32_t Texture::getMaxAnisotropy() const
        {
            return resource->getMaxAnisotropy();
        }

        bool Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            if (!resource->setMaxAnisotropy(newMaxAnisotropy))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        uint32_t Texture::getSampleCount() const
        {
            return resource->getSampleCount();
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
            resource->setClearColorBuffer(clear);

            sharedEngine->getRenderer()->uploadResource(resource);
        }

        bool Texture::getClearDepthBuffer() const
        {
            return resource->getClearDepthBuffer();
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            resource->setClearDepthBuffer(clear);

            sharedEngine->getRenderer()->uploadResource(resource);
        }

        Color Texture::getClearColor() const
        {
            return resource->getClearColor();
        }

        void Texture::setClearColor(Color color)
        {
            resource->setClearColor(color);

            sharedEngine->getRenderer()->uploadResource(resource);
        }
    } // namespace graphics
} // namespace ouzel
