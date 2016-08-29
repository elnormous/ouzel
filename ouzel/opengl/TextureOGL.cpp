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
        }

        bool TextureOGL::upload()
        {
            if (dirty)
            {
                if (!textureId)
                {
                    glGenTextures(1, &textureId);

                    if (RendererOGL::checkOpenGLError())
                    {
                        log("Failed to create texture");
                        return false;
                    }
                }

                if (!uploadData.levels.empty())
                {
                    RendererOGL::bindTexture(textureId, 0);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    if (uploadData.levels.size() > 1) // has mip-maps
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

                    if (RendererOGL::checkOpenGLError())
                    {
                        log("Failed to set texture parameters");
                        return false;
                    }

                    for (size_t level = 0; level < uploadData.levels.size(); ++level)
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), GL_RGBA,
                                     static_cast<GLsizei>(uploadData.levels[level].size.width),
                                     static_cast<GLsizei>(uploadData.levels[level].size.height), 0,
                                     GL_RGBA, GL_UNSIGNED_BYTE, uploadData.levels[level].data.data());

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to upload texture data");
                            return false;
                        }
                    }

                }

                ready = true;
                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
