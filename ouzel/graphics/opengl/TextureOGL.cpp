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
                RendererOGL::deleteRenderBuffer(depthBufferId);
            }

            if (frameBufferId)
            {
                RendererOGL::deleteFrameBuffer(frameBufferId);
            }

            if (textureId)
            {
                RendererOGL::deleteTexture(textureId);
            }
        }

        bool TextureOGL::upload()
        {
            if (!TextureResource::upload())
            {
                return false;
            }

            if (data.dirty)
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

                RendererOGL::bindTexture(textureId, 0);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());

                switch (rendererOGL->getTextureFilter())
                {
                    case Texture::Filter::NONE:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.mipMapsGenerated ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case Texture::Filter::LINEAR:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.mipMapsGenerated ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        break;
                    case Texture::Filter::BILINEAR:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.mipMapsGenerated ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                    case Texture::Filter::TRILINEAR:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.mipMapsGenerated ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        break;
                }

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture parameters";
                    return false;
                }

                if (static_cast<GLsizei>(data.size.v[0]) != width ||
                    static_cast<GLsizei>(data.size.v[1]) != height)
                {
                    width = static_cast<GLsizei>(data.size.v[0]);
                    height = static_cast<GLsizei>(data.size.v[1]);

                    for (size_t level = 0; level < data.levels.size(); ++level)
                    {
                        if (!data.levels[level].data.empty())
                        {
                            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), GL_RGBA,
                                         static_cast<GLsizei>(data.levels[level].size.v[0]),
                                         static_cast<GLsizei>(data.levels[level].size.v[1]), 0,
                                         GL_RGBA, GL_UNSIGNED_BYTE, data.levels[level].data.data());
                        }
                    }

                    if (data.renderTarget)
                    {
                        if (!frameBufferId)
                        {
                            glGenFramebuffersProc(1, &frameBufferId);

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to create frame buffer";
                                return false;
                            }

                            clearMask = 0;

                            if (data.clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
                            if (data.clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;
                        }

                        RendererOGL::bindFrameBuffer(frameBufferId);

                        if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                        {
                            RendererOGL::bindTexture(textureId, 0);

                            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                                         static_cast<GLsizei>(data.size.v[0]),
                                         static_cast<GLsizei>(data.size.v[1]),
                                         0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

                            if (data.depth)
                            {
                                glGenRenderbuffersProc(1, &depthBufferId);
                                glBindRenderbufferProc(GL_RENDERBUFFER, depthBufferId);
                                glRenderbufferStorageProc(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                                          static_cast<GLsizei>(data.size.v[0]),
                                                          static_cast<GLsizei>(data.size.v[1]));
                                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);
                            }

                            glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    #if OUZEL_SUPPORTS_OPENGL // TODO: fix this
                            //GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
                            //glDrawBuffers(1, drawBuffers);
                            glDrawBuffer(GL_COLOR_ATTACHMENT0);
    #endif

                            if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                            {
                                Log(Log::Level::ERR) << "Failed to create frame buffer";
                                return false;
                            }
                        }

                        frameBufferClearColor[0] = data.clearColor.normR();
                        frameBufferClearColor[1] = data.clearColor.normG();
                        frameBufferClearColor[2] = data.clearColor.normB();
                        frameBufferClearColor[3] = data.clearColor.normA();
                    }
                }
                else
                {
                    for (size_t level = 0; level < data.levels.size(); ++level)
                    {
                        if (!data.levels[level].data.empty())
                        {
                            glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                            static_cast<GLsizei>(data.levels[level].size.v[0]),
                                            static_cast<GLsizei>(data.levels[level].size.v[1]),
                                            GL_RGBA, GL_UNSIGNED_BYTE, data.levels[level].data.data());

                            if (RendererOGL::checkOpenGLError())
                            {
                                Log(Log::Level::ERR) << "Failed to upload texture data";
                                return false;
                            }
                        }
                    }
                }

                data.dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
