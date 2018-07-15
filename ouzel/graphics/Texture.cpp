// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Texture.hpp"
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "ImageDataSTB.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"

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

        void Texture::init(const Size2& newSize,
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

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<void(TextureResource::*)(const Size2&, uint32_t, uint32_t, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   newSize,
                                                                   newFlags,
                                                                   newMipmaps,
                                                                   newSampleCount,
                                                                   newPixelFormat));
        }

        void Texture::init(const std::string& filename,
                           uint32_t newFlags,
                           uint32_t newMipmaps,
                           PixelFormat newPixelFormat)
        {
            ImageDataSTB image(filename, newPixelFormat);

            size = image.getSize();
            flags = newFlags;
            mipmaps = newMipmaps;
            sampleCount = 1;
            pixelFormat = image.getPixelFormat();

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<void(TextureResource::*)(const std::vector<uint8_t>&, const Size2&, uint32_t, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   image.getData(),
                                                                   image.getSize(),
                                                                   newFlags,
                                                                   newMipmaps,
                                                                   image.getPixelFormat()));
        }

        void Texture::init(const std::vector<uint8_t>& newData,
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

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<void(TextureResource::*)(const std::vector<uint8_t>&, const Size2&, uint32_t, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   newData,
                                                                   newSize,
                                                                   newFlags,
                                                                   newMipmaps,
                                                                   newPixelFormat));
        }

        void Texture::init(const std::vector<Level>& newLevels,
                           const Size2& newSize,
                           uint32_t newFlags,
                           PixelFormat newPixelFormat)
        {
            size = newSize;
            flags = newFlags;
            mipmaps = static_cast<uint32_t>(newLevels.size());
            sampleCount = 1;
            pixelFormat = newPixelFormat;

            if ((flags & RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");

            engine->getRenderer()->executeOnRenderThread(std::bind(static_cast<void(TextureResource::*)(const std::vector<Level>&, const Size2&, uint32_t, PixelFormat)>(&TextureResource::init),
                                                                   resource,
                                                                   newLevels,
                                                                   newSize,
                                                                   newFlags,
                                                                   newPixelFormat));
        }

        void Texture::setSize(const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC))
                throw DataError("Texture is not dynamic");

            if (newSize.width <= 0.0F || newSize.height <= 0.0F)
                throw DataError("Invalid texture size");

            size = newSize;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setSize,
                                                                   resource,
                                                                   newSize));
        }

        void Texture::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw DataError("Texture is not dynamic");

            if (newSize.width <= 0.0F || newSize.height <= 0.0F)
                throw DataError("Invalid texture size");

            size = newSize;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setData,
                                                                   resource,
                                                                   newData,
                                                                   newSize));
        }

        void Texture::setFilter(Filter newFilter)
        {
            filter = newFilter;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setFilter,
                                                                   resource,
                                                                   newFilter));
        }

        void Texture::setAddressX(Address newAddressX)
        {
            addressX = newAddressX;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setAddressX,
                                                                   resource,
                                                                   newAddressX));
        }

        void Texture::setAddressY(Address newAddressY)
        {
            addressY = newAddressY;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setAddressY,
                                                                   resource,
                                                                   newAddressY));
        }

        void Texture::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;

            engine->getRenderer()->executeOnRenderThread(std::bind(&TextureResource::setMaxAnisotropy,
                                                                   resource,
                                                                   newMaxAnisotropy));
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
