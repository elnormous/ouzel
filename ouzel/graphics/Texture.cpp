// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Texture.hpp"
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "ImageDataSTB.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace graphics
    {
        Texture::Texture()
        {
            resource = engine->getRenderer()->getDevice()->createTexture();
        }

        Texture::~Texture()
        {
            if (engine && resource) engine->getRenderer()->getDevice()->deleteResource(resource);
        }

        bool Texture::init(const Size2& newSize,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           uint32_t newSampleCount,
                           PixelFormat newPixelFormat)
        {
            size = newSize;
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = newSampleCount;
            pixelFormat = newPixelFormat;

            if ((flags & DYNAMIC || flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                return false;

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureResource::*)(const Size2&, uint32_t, uint32_t, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   newSize,
                                                                   newFlags,
                                                                   newMipmaps,
                                                                   newSampleCount,
                                                                   newPixelFormat));

            return true;
        }

        bool Texture::init(const std::string& filename,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           PixelFormat newPixelFormat)
        {
            ImageDataSTB image;
            image.init(filename, newPixelFormat);

            size = image.getSize();
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = image.getPixelFormat();

            if ((flags & DYNAMIC || flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                return false;

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureResource::*)(const std::vector<uint8_t>&, const Size2&, uint32_t, uint32_t, PixelFormat)>(&TextureResource::init),
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
            size = newSize;
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            if ((flags & DYNAMIC || flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                return false;

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureResource::*)(const std::vector<uint8_t>&, const Size2&, uint32_t, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   newData,
                                                                   newSize,
                                                                   newFlags,
                                                                   newMipmaps,
                                                                   newPixelFormat));

            return true;
        }

        bool Texture::init(const std::vector<Level>& newLevels,
                           const Size2& newSize,
                           uint32_t newFlags,
                           PixelFormat newPixelFormat)
        {
            size = newSize;
            flags = newFlags;
            mipmaps = static_cast<uint32_t>(newLevels.size());
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            if ((flags & DYNAMIC || flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                return false;

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<bool(TextureResource::*)(const std::vector<Level>&, const Size2&, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   newLevels,
                                                                   newSize,
                                                                   newFlags,
                                                                   newPixelFormat));

            return true;
        }

        bool Texture::setSize(const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC))
                return false;

            size = newSize;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setSize,
                                                                   resource,
                                                                   newSize));

            return true;
        }

        bool Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                return false;

            size = newSize;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setData,
                                                                   resource,
                                                                   newData,
                                                                   newSize));

            return true;
        }

        bool Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setFilter,
                                                                   resource,
                                                                   newFilter));

            return true;
        }

        bool Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setAddressX,
                                                                   resource,
                                                                   newAddressX));

            return true;
        }

        bool Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setAddressY,
                                                                   resource,
                                                                   newAddressY));

            return true;
        }

        bool Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setMaxAnisotropy,
                                                                   resource,
                                                                   newMaxAnisotropy));

            return true;
        }

        void Texture::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setClearColorBuffer,
                                                                   resource,
                                                                   clear));
        }

        void Texture::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setClearDepthBuffer,
                                                                   resource,
                                                                   clear));
        }

        void Texture::setClearColor(Color color)
        {
            clearColor = color;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setClearColor,
                                                                   resource,
                                                                   color));
        }

        void Texture::setClearDepth(float depth)
        {
            clearDepth = depth;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setClearDepth,
                                                                   resource,
                                                                   depth));
        }

    } // namespace graphics
} // namespace ouzel
