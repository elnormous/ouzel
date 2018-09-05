// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "TextureResource.hpp"
#include "Renderer.hpp"
#include "RenderDevice.hpp"
#include "core/Engine.hpp"
#include "math/MathUtils.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        TextureResource::TextureResource(RenderDevice& initRenderDevice):
            renderDevice(initRenderDevice)
        {
        }

        TextureResource::~TextureResource()
        {
        }

        void TextureResource::init(const std::vector<Texture::Level>& newLevels,
                                   uint32_t newFlags,
                                   uint32_t newSampleCount,
                                   PixelFormat newPixelFormat)
        {
            levels = newLevels;
            size = newLevels.front().size;
            flags = newFlags;
            mipmaps = static_cast<uint32_t>(newLevels.size());
            sampleCount = newSampleCount;
            pixelFormat = newPixelFormat;

            if ((flags & Texture::RENDER_TARGET) && (mipmaps == 0 || mipmaps > 1))
                throw DataError("Invalid mip map count");
        }

        void TextureResource::setData(const std::vector<Texture::Level>& newLevels)
        {
            if (!(flags & Texture::DYNAMIC) || flags & Texture::RENDER_TARGET)
                throw DataError("Texture is not dynamic");

            levels = newLevels;
        }

        void TextureResource::setFilter(Texture::Filter newFilter)
        {
            filter = newFilter;
        }

        void TextureResource::setAddressX(Texture::Address newAddressX)
        {
            addressX = newAddressX;
        }

        void TextureResource::setAddressY(Texture::Address newAddressY)
        {
            addressY = newAddressY;
        }

        void TextureResource::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            maxAnisotropy = newMaxAnisotropy;
        }

        void TextureResource::setClearColorBuffer(bool clear)
        {
            clearColorBuffer = clear;
        }

        void TextureResource::setClearDepthBuffer(bool clear)
        {
            clearDepthBuffer = clear;
        }

        void TextureResource::setClearColor(Color color)
        {
            clearColor = color;
        }

        void TextureResource::setClearDepth(float clear)
        {
            clearDepth = clear;
        }
    } // namespace graphics
} // namespace ouzel
