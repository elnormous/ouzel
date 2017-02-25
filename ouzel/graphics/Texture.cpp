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
            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = newRenderTarget;
            sampleCount = newSampleCount;
            depth = newDepth;

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

            size = image.getSize();
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = false;
            sampleCount = 1;
            depth = false;

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
            size = newSize;
            dynamic = newDynamic;
            mipmaps = newMipmaps;
            renderTarget = false;
            sampleCount = 1;
            depth = false;

            if (!resource->initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::setSize(const Size2& newSize)
        {
            size = newSize;
            if (!resource->setSize(newSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            size = newSize;
            if (!resource->setData(newData, newSize))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;
            if (!resource->setFilter(newFilter))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::setWrapMode(WrapMode newWrapMode)
        {
            wrapMode = newWrapMode;
            if (!resource->setWrapMode(newWrapMode))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        bool Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;
            if (!resource->setMaxAnisotropy(newMaxAnisotropy))
            {
                return false;
            }

            sharedEngine->getRenderer()->uploadResource(resource);

            return true;
        }

        void Texture::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;
            resource->setClearColorBuffer(clear);

            sharedEngine->getRenderer()->uploadResource(resource);
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;
            resource->setClearDepthBuffer(clear);

            sharedEngine->getRenderer()->uploadResource(resource);
        }

        void Texture::setClearColor(Color color)
        {
            clearColor = color;
            resource->setClearColor(color);

            sharedEngine->getRenderer()->uploadResource(resource);
        }
    } // namespace graphics
} // namespace ouzel
