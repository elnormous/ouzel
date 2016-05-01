// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "TextureOGL.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Image.h"
#include "MathUtils.h"
#include "stb_image_resize.h"

namespace ouzel
{
    namespace graphics
    {
        TextureOGL::TextureOGL()
        {

        }

        TextureOGL::~TextureOGL()
        {
            destroy();
        }

        void TextureOGL::destroy()
        {
            if (textureId)
            {
                glDeleteTextures(1, &textureId);
                textureId = 0;
            }
        }

        bool TextureOGL::init(const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::init(newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            glGenTextures(1, &textureId);

            if (size.width > 0.0f && size.height > 0.0f)
            {
                RendererOGL::bindTexture(textureId, 0);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                             static_cast<GLsizei>(size.width),
                             static_cast<GLsizei>(size.height),
                             0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

                if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
                {
                    return false;
                }
            }

            return true;
        }

        bool TextureOGL::initFromData(const void* data, const Size2& newSize, bool newDynamic, bool newMipmaps)
        {
            if (!Texture::initFromData(data, newSize, newDynamic, newMipmaps))
            {
                return false;
            }

            destroy();

            glGenTextures(1, &textureId);

            RendererOGL::bindTexture(textureId, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return uploadData(data,
                              static_cast<GLsizei>(size.width),
                              static_cast<GLsizei>(size.height));
        }

        bool TextureOGL::upload(const void* data, const Size2& newSize)
        {
            if (!Texture::upload(data, newSize))
            {
                return false;
            }

            return uploadData(data,
                              static_cast<GLsizei>(size.width),
                              static_cast<GLsizei>(size.height));
        }

        bool TextureOGL::uploadData(const void* data, GLsizei newWidth, GLsizei newHeight)
        {
            if (newWidth <= 0 || newHeight <= 0)
            {
                return false;
            }

            RendererOGL::bindTexture(textureId, 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, data);

#ifdef OUZEL_SUPPORTS_OPENGLES
            if (mipmaps && isPOT(width) && isPOT(height))
#else
            if (mipmaps)
#endif
            {
                GLsizei oldMipWidth = newWidth;
                GLsizei oldMipHeight = newHeight;

                GLsizei mipWidth = newWidth >> 1;
                GLsizei mipHeight = newHeight >> 1;
                if (mipWidth < 1) mipWidth = 1;
                if (mipHeight < 1) mipHeight = 1;
                GLint mipLevel = 1;

                std::vector<uint8_t> oldMipMapData(newWidth * newHeight * 4);
                memcpy(oldMipMapData.data(), data, newWidth * newHeight * 4);

                std::vector<uint8_t> newMipMapData(mipWidth * mipHeight * 4);

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;

                    stbir_resize_uint8_generic(oldMipMapData.data(), oldMipWidth, oldMipHeight, 0,
                                               newMipMapData.data(), mipWidth, mipHeight, 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, mipWidth, mipHeight,
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, newMipMapData.data());

                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;

                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    ++mipLevel;

                    newMipMapData.swap(oldMipMapData);
                }

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
