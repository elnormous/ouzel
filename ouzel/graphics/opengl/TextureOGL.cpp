// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "TextureOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "graphics/Image.h"
#include "utils/Log.h"

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
                RendererOGL::deleteResource(textureId, RendererOGL::ResourceType::Texture);
            }
        }

        void TextureOGL::free()
        {
            Texture::free();

            if (textureId)
            {
                RendererOGL::deleteResource(textureId, RendererOGL::ResourceType::Texture);
                textureId = 0;
            }

            width = 0;
            height = 0;
        }

        bool TextureOGL::upload()
        {
            if (uploadData.dirty)
            {
                if (!textureId)
                {
                    glGenTextures(1, &textureId);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create texture";
                        return false;
                    }
                }

                if (!uploadData.levels.empty())
                {
                    RendererOGL::bindTexture(textureId, 0);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    if (uploadData.mipmaps) // has mip-maps
                    {
                        RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());

                        switch (rendererOGL->getTextureFilter())
                        {
                            case TextureFilter::NONE:
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                                break;
                            case TextureFilter::LINEAR:
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                                break;
                            case TextureFilter::BILINEAR:
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                                break;
                            case TextureFilter::TRILINEAR:
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                                break;
                        }
                    }
                    else
                    {
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    }

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture parameters";
                        return false;
                    }

                    if (static_cast<GLsizei>(uploadData.size.width) != width ||
                        static_cast<GLsizei>(uploadData.size.height) != height)
                    {
                        width = static_cast<GLsizei>(uploadData.size.width);
                        height = static_cast<GLsizei>(uploadData.size.height);

                        for (size_t level = 0; level < uploadData.levels.size(); ++level)
                        {
                            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), GL_RGBA,
                                         static_cast<GLsizei>(uploadData.levels[level].size.width),
                                         static_cast<GLsizei>(uploadData.levels[level].size.height), 0,
                                         GL_RGBA, GL_UNSIGNED_BYTE, uploadData.levels[level].data.data());
                        }
                    }
                    else
                    {
                        for (size_t level = 0; level < uploadData.levels.size(); ++level)
                        {
                            glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                            static_cast<GLsizei>(uploadData.levels[level].size.width),
                                            static_cast<GLsizei>(uploadData.levels[level].size.height),
                                            GL_RGBA, GL_UNSIGNED_BYTE, uploadData.levels[level].data.data());

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to upload texture data";
                                return false;
                            }
                        }
                    }
                }

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
