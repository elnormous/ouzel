// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureOGL.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Image.h"
#include "MathUtils.h"
#include "stb_image_resize.h"

namespace ouzel
{
    namespace video
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
            if (_textureId)
            {
                glDeleteTextures(1, &_textureId);
                _textureId = 0;
            }
        }

        bool TextureOGL::init(const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::init(size, dynamic, mipmaps))
            {
                return false;
            }

            destroy();

            glGenTextures(1, &_textureId);

            if (size.width > 0.0f && size.height > 0.0f)
            {
                RendererOGL::bindTexture(_textureId, 0);

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

        bool TextureOGL::initFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            if (!Texture::initFromData(data, size, dynamic, mipmaps))
            {
                return false;
            }

            destroy();

            glGenTextures(1, &_textureId);

            RendererOGL::bindTexture(_textureId, 0);

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

        bool TextureOGL::upload(const void* data, const Size2& size)
        {
            if (!Texture::upload(data, size))
            {
                return false;
            }

            return uploadData(data,
                              static_cast<GLsizei>(size.width),
                              static_cast<GLsizei>(size.height));
        }

        bool TextureOGL::uploadData(const void* data, GLsizei width, GLsizei height)
        {
            if (width <= 0 || height <= 0)
            {
                return false;
            }

            RendererOGL::bindTexture(_textureId, 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, data);

#ifdef OUZEL_SUPPORTS_OPENGLES
            if (_mipmaps && isPOT(width) && isPOT(height))
#else
            if (_mipmaps)
#endif
            {
                GLsizei oldMipWidth = width;
                GLsizei oldMipHeight = height;

                GLsizei mipWidth = width >> 1;
                GLsizei mipHeight = height >> 1;
                GLint mipLevel = 1;

                uint8_t* oldMipMapData = new uint8_t[width * height * 4];
                memcpy(oldMipMapData, data, static_cast<size_t>(width * height * 4));

                uint8_t* newMipMapData = new uint8_t[mipWidth * mipHeight * 4];

                while (mipWidth >= 1 || mipHeight >= 1)
                {
                    if (mipWidth < 1) mipWidth = 1;
                    if (mipHeight < 1) mipHeight = 1;

                    stbir_resize_uint8_generic(oldMipMapData, oldMipWidth, oldMipHeight, 0,
                                               newMipMapData, mipWidth, mipHeight, 0, 4,
                                               3, 0, STBIR_EDGE_CLAMP,
                                               STBIR_FILTER_TRIANGLE, STBIR_COLORSPACE_LINEAR, nullptr);

                    glTexImage2D(GL_TEXTURE_2D, mipLevel, GL_RGBA, mipWidth, mipHeight,
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, newMipMapData);

                    oldMipWidth = mipWidth;
                    oldMipHeight = mipHeight;

                    mipWidth >>= 1;
                    mipHeight >>= 1;
                    mipLevel++;

                    uint8_t* temp = oldMipMapData;
                    oldMipMapData = newMipMapData;
                    newMipMapData = temp;
                }

                delete [] oldMipMapData;
                delete [] newMipMapData;

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
    } // namespace video
} // namespace ouzel
