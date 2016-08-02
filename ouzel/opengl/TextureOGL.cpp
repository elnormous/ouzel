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
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Texture::init(newSize, newDynamic, newMipmaps, newRenderTarget))
            {
                return false;
            }

            ready = true;

            return true;
        }

        bool TextureOGL::initFromBuffer(const void* newData, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            
            if (!Texture::initFromBuffer(newData, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            data.assign(static_cast<const uint8_t*>(newData),
                        static_cast<const uint8_t*>(newData) + static_cast<int>(newSize.width) * static_cast<int>(newSize.height) * 4);

            ready = true;

            return true;
        }

        bool TextureOGL::uploadMipmap(uint32_t level, const void* newData)
        {
            if (!Texture::uploadMipmap(level, newData))
            {
                return false;
            }

            GLsizei newWidth = static_cast<GLsizei>(mipmapSizes[level].width);
            GLsizei newHeight = static_cast<GLsizei>(mipmapSizes[level].height);

            RendererOGL::bindTexture(textureId, 0);

            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), GL_RGBA, newWidth, newHeight, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, newData);

            if (RendererOGL::checkOpenGLError())
            {
                log("Failed to upload texture data");
                return false;
            }

            return true;
        }

        bool TextureOGL::uploadData(const void* newData, const Size2& newSize)
        {
            if (!Texture::uploadData(newData, newSize))
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

        bool TextureOGL::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                if (!textureId)
                {
                    glGenTextures(1, &textureId);

                    if (RendererOGL::checkOpenGLError())
                    {
                        log("Failed to create texture");
                        return false;
                    }

                    RendererOGL::bindTexture(textureId, 0);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    if (RendererOGL::checkOpenGLError())
                    {
                        log("Failed to set texture parameters");
                        return false;
                    }
                }

                if (!data.empty())
                {
                    return uploadData(data.data(), size);
                }

                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
