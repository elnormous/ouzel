// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "TextureOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "graphics/Image.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        TextureOGL::TextureOGL()
        {

        }

        TextureOGL::~TextureOGL()
        {
            if (textureId)
            {
                RendererOGL::unbindTexture(textureId);

                glDeleteTextures(1, &textureId);
            }
        }

        void TextureOGL::free()
        {
            Texture::free();

            if (textureId)
            {
                RendererOGL::unbindTexture(textureId);

                glDeleteTextures(1, &textureId);
                textureId = 0;
            }
        }

        bool TextureOGL::init(const Size2& newSize, bool newDynamic, bool newMipmaps, bool newRenderTarget)
        {
            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            free();

            glGenTextures(1, &textureId);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                log("Failed to create texture");
                return false;
            }

            ready = true;

            return true;
        }

        bool TextureOGL::initFromBuffer(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromBuffer(data, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            free();

            glGenTextures(1, &textureId);

            RendererOGL::bindTexture(textureId, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                log("Failed to create texture");
                return false;
            }

            ready = true;

            return uploadData(data, size);
        }

        bool TextureOGL::uploadMipmap(uint32_t level, const void* data)
        {
            if (!Texture::uploadMipmap(level, data))
            {
                return false;
            }

            GLsizei newWidth = static_cast<GLsizei>(mipmapSizes[level].width);
            GLsizei newHeight = static_cast<GLsizei>(mipmapSizes[level].height);

            RendererOGL::bindTexture(textureId, 0);

            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), GL_RGBA, newWidth, newHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, data);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                log("Failed to upload texture data");
                return false;
            }

            return true;
        }

        bool TextureOGL::uploadData(const void* data, const Size2& newSize)
        {
            if (!Texture::uploadData(data, newSize))
            {
                return false;
            }

            if (mipmapSizes.size() > 1) // has mip-maps
            {
                std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

                switch (rendererOGL->getTextureFiltering())
                {
                    case Renderer::TextureFiltering::NONE:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                        break;
                    case Renderer::TextureFiltering::LINEAR:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                        break;
                    case Renderer::TextureFiltering::BILINEAR:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                        break;
                    case Renderer::TextureFiltering::TRILINEAR:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        break;
                }
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
