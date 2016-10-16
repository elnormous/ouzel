// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureMetal.h"
#include "core/Engine.h"
#include "RendererMetal.h"
#include "graphics/Image.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace graphics
    {
        TextureMetal::TextureMetal()
        {
        }

        TextureMetal::~TextureMetal()
        {
            if (texture)
            {
                [texture release];
            }
        }

        void TextureMetal::free()
        {
            Texture::free();

            if (texture)
            {
                [texture release];
                texture = Nil;
            }

            width = 0;
            height = 0;
        }

        bool TextureMetal::upload()
        {
            if (uploadData.dirty)
            {
                RendererMetal* rendererMetal = static_cast<RendererMetal*>(sharedEngine->getRenderer());

                if (uploadData.size.width > 0 &&
                    uploadData.size.height > 0)
                {
                    if (!texture ||
                        static_cast<NSUInteger>(uploadData.size.width) != width ||
                        static_cast<NSUInteger>(uploadData.size.height) != height)
                    {
                        if (texture) [texture release];

                        width = static_cast<NSUInteger>(uploadData.size.width);
                        height = static_cast<NSUInteger>(uploadData.size.height);

                        if (width > 0 && height > 0)
                        {
                            MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:uploadData.renderTarget ? rendererMetal->getMetalView().colorPixelFormat : MTLPixelFormatRGBA8Unorm
                                                                                                                         width:width
                                                                                                                        height:height
                                                                                                                     mipmapped:uploadData.mipmaps ? YES : NO];
                            textureDescriptor.textureType = MTLTextureType2D;
                            textureDescriptor.usage = MTLTextureUsageShaderRead | (uploadData.renderTarget ? MTLTextureUsageRenderTarget : 0);

                            texture = [rendererMetal->getDevice() newTextureWithDescriptor:textureDescriptor];

                            if (!texture)
                            {
                                return false;
                            }
                        }
                    }

                    for (size_t level = 0; level < uploadData.levels.size(); ++level)
                    {
                        [texture replaceRegion:MTLRegionMake2D(0, 0,
                                                               static_cast<NSUInteger>(uploadData.levels[level].size.width),
                                                               static_cast<NSUInteger>(uploadData.levels[level].size.height))
                                   mipmapLevel:level withBytes:uploadData.levels[level].data.data()
                                   bytesPerRow:static_cast<NSUInteger>(uploadData.levels[level].pitch)];
                    }
                }

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
