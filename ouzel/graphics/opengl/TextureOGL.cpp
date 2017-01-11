// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            if (depthBufferId)
            {
                RendererOGL::deleteResource(depthBufferId, RendererOGL::ResourceType::RenderBuffer);
            }

            if (frameBufferId)
            {
                RendererOGL::deleteResource(frameBufferId, RendererOGL::ResourceType::FrameBuffer);
            }

            if (textureId)
            {
                RendererOGL::deleteResource(textureId, RendererOGL::ResourceType::Texture);
            }
        }

        void TextureOGL::free()
        {
            Texture::free();

            if (depthBufferId)
            {
                RendererOGL::deleteResource(depthBufferId, RendererOGL::ResourceType::RenderBuffer);
                depthBufferId = 0;
            }

            if (frameBufferId)
            {
                RendererOGL::deleteResource(frameBufferId, RendererOGL::ResourceType::FrameBuffer);
                frameBufferId = 0;
            }

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
            if (!Texture::upload())
            {
                return false;
            }

            if (!textureId)
            {
                glGenTextures(1, &textureId);

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create texture";
                    return false;
                }
            }

            RendererOGL::bindTexture(textureId, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());

            switch (rendererOGL->getTextureFilter())
            {
                case TextureFilter::NONE:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uploadData.mipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case TextureFilter::LINEAR:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uploadData.mipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                case TextureFilter::BILINEAR:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uploadData.mipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                case TextureFilter::TRILINEAR:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uploadData.mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
            }

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture parameters";
                return false;
            }

            if (static_cast<GLsizei>(uploadData.size.v[0]) != width ||
                static_cast<GLsizei>(uploadData.size.v[1]) != height)
            {
                width = static_cast<GLsizei>(uploadData.size.v[0]);
                height = static_cast<GLsizei>(uploadData.size.v[1]);

                for (size_t level = 0; level < uploadData.levels.size(); ++level)
                {
                    if (!uploadData.levels[level].data.empty())
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), GL_RGBA,
                                     static_cast<GLsizei>(uploadData.levels[level].size.v[0]),
                                     static_cast<GLsizei>(uploadData.levels[level].size.v[1]), 0,
                                     GL_RGBA, GL_UNSIGNED_BYTE, uploadData.levels[level].data.data());
                    }
                }

                if (uploadData.renderTarget)
                {
                    if (!frameBufferId)
                    {
                        glGenFramebuffers(1, &frameBufferId);

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to create frame buffer";
                            return false;
                        }

                        clearMask = 0;

                        if (uploadData.clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
                        if (uploadData.clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;
                    }

                    RendererOGL::bindFrameBuffer(frameBufferId);

                    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    {
                        RendererOGL::bindTexture(textureId, 0);

                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                                     static_cast<GLsizei>(uploadData.size.v[0]),
                                     static_cast<GLsizei>(uploadData.size.v[1]),
                                     0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                        if (uploadData.depthBits > 0)
                        {
                            glGenRenderbuffers(1, &depthBufferId);
                            glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
                            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                                  static_cast<GLsizei>(uploadData.size.v[0]),
                                                  static_cast<GLsizei>(uploadData.size.v[1]));
                            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);
                        }

                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

#if OUZEL_SUPPORTS_OPENGL // TODO: fix this
                        //GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
                        //glDrawBuffers(1, drawBuffers);
                        glDrawBuffer(GL_COLOR_ATTACHMENT0);
#endif

                        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                        {
                            Log(Log::Level::ERR) << "Failed to create frame buffer";
                            return false;
                        }
                    }
                    
                    frameBufferClearColor[0] = uploadData.clearColor.normR();
                    frameBufferClearColor[1] = uploadData.clearColor.normG();
                    frameBufferClearColor[2] = uploadData.clearColor.normB();
                    frameBufferClearColor[3] = uploadData.clearColor.normA();
                }
            }
            else
            {
                for (size_t level = 0; level < uploadData.levels.size(); ++level)
                {
                    if (!uploadData.levels[level].data.empty())
                    {
                        glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                        static_cast<GLsizei>(uploadData.levels[level].size.v[0]),
                                        static_cast<GLsizei>(uploadData.levels[level].size.v[1]),
                                        GL_RGBA, GL_UNSIGNED_BYTE, uploadData.levels[level].data.data());

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to upload texture data";
                            return false;
                        }
                    }
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
