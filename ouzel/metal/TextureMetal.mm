// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureMetal.h"
#include "Engine.h"
#include "RendererMetal.h"
#include "Image.h"
#include "MathUtils.h"
#include "stb_image_resize.h"

namespace ouzel
{
    namespace video
    {
        TextureMetal::TextureMetal()
        {

        }

        TextureMetal::~TextureMetal()
        {
            destroy();
        }

        void TextureMetal::destroy()
        {
            if (_texture) [_texture release];
        }

        bool TextureMetal::init(const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::init(size, dynamic, mipmaps))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                         width:static_cast<NSUInteger>(size.width)
                                                                                                        height:static_cast<NSUInteger>(size.height)
                                                                                                     mipmapped:mipmaps ? YES : NO];
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.usage = MTLTextureUsageShaderRead;

            _texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

            return true;
        }

        bool TextureMetal::initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::initFromData(data, size, dynamic, mipmaps))
            {
                return false;
            }

            destroy();

            std::shared_ptr<RendererMetal> rendererMetal = std::static_pointer_cast<RendererMetal>(sharedEngine->getRenderer());

            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                         width:static_cast<NSUInteger>(size.width)
                                                                                                        height:static_cast<NSUInteger>(size.height)
                                                                                                     mipmapped:mipmaps ? YES : NO];
            textureDescriptor.textureType = MTLTextureType2D;
            textureDescriptor.usage = MTLTextureUsageShaderRead;

            _texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

            return true;
        }

        bool TextureMetal::upload(const void* data, const Size2& size)
        {
            if (!Texture::upload(data, size))
            {
                return false;
            }

            return true;
        }
    } // namespace video
} // namespace ouzel
