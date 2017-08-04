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

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureInterface::*)(const Size2&, uint32_t, uint32_t, uint32_t, PixelFormat)>(&TextureInterface::init),
                                                                         resource,
                                                                         newSize,
                                                                         newFlags,
                                                                         newMipmaps,
                                                                         newSampleCount,
                                                                         newPixelFormat));

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

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureInterface::*)(const std::vector<uint8_t>&, const Size2&, uint32_t, uint32_t, PixelFormat)>(&TextureInterface::init),
                                                                         resource,
                                                                         image.getData(),
                                                                         image.getSize(),
                                                                         newFlags,
                                                                         newMipmaps,
                                                                         image.getPixelFormat()));

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

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureInterface::*)(const std::vector<uint8_t>&, const Size2&, uint32_t, uint32_t, PixelFormat)>(&TextureInterface::init),
                                                                         resource,
                                                                         newData,
                                                                         newSize,
                                                                         newFlags,
                                                                         newMipmaps,
                                                                         newPixelFormat));

            return true;
        }

        const Size2& Texture::getSize() const
        {
            return size;
        }

        bool Texture::setSize(const Size2& newSize)
        {
            size = newSize;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setSize,
                                                                         resource,
                                                                         newSize));

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            size = newSize;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setData,
                                                                         resource,
                                                                         newData,
                                                                         newSize));

            return true;
        }

        Texture::Filter Texture::getFilter() const
        {
            return filter;
        }

        bool Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setFilter,
                                                                         resource,
                                                                         newFilter));

            return true;
        }

        Texture::Address Texture::getAddressX() const
        {
            return addressX;
        }

        bool Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setAddressX,
                                                                         resource,
                                                                         newAddressX));

            return true;
        }

        Texture::Address Texture::getAddressY() const
        {
            return addressY;
        }

        bool Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setAddressY,
                                                                         resource,
                                                                         newAddressY));

            return true;
        }

        uint32_t Texture::getMaxAnisotropy() const
        {
            return maxAnisotropy;
        }

        bool Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setMaxAnisotropy,
                                                                         resource,
                                                                         newMaxAnisotropy));

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

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setClearColorBuffer,
                                                                         resource,
                                                                         clear));
        }

        bool Texture::getClearDepthBuffer() const
        {
            return clearDepthBuffer;
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setClearDepthBuffer,
                                                                         resource,
                                                                         clear));
        }

        Color Texture::getClearColor() const
        {
            return clearColor;
        }

        void Texture::setClearColor(Color color)
        {
            clearColor = color;

            sharedEngine->getRenderer()->executeOnRenderThread(std::bind(&TextureInterface::setClearColor,
                                                                         resource,
                                                                         color));
        }
    } // namespace graphics
} // namespace ouzel
