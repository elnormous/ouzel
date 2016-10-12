// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetOGL.h"
#include "TextureOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetOGL::RenderTargetOGL()
        {
        }

        RenderTargetOGL::~RenderTargetOGL()
        {
            if (depthBufferId)
            {
                RendererOGL::deleteResource(depthBufferId, RendererOGL::ResourceType::RenderBuffer);
            }

            if (frameBufferId)
            {
                RendererOGL::deleteResource(frameBufferId, RendererOGL::ResourceType::FrameBuffer);
            }
        }

        void RenderTargetOGL::free()
        {
            RenderTarget::free();

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
        }

        bool RenderTargetOGL::upload()
        {
            if (uploadData.dirty)
            {
                if (!texture->upload())
                {
                    return false;
                }

                if (!frameBufferId)
                {
                    glGenFramebuffers(1, &frameBufferId);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create frame buffer";
                        return false;
                    }

                    clearMask = GL_COLOR_BUFFER_BIT;

                    if (uploadData.depthBuffer)
                    {
                        clearMask |= GL_DEPTH_BUFFER_BIT;
                    }
                }

                std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(texture);

                if (!textureOGL->getTextureId())
                {
                    Log(Log::Level::ERR) << "OpenGL texture not initialized";
                    return false;
                }

                RendererOGL::bindFrameBuffer(frameBufferId);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    RendererOGL::bindTexture(textureOGL->getTextureId(), 0);

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                                 static_cast<GLsizei>(uploadData.size.width),
                                 static_cast<GLsizei>(uploadData.size.height),
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                    if (uploadData.depthBuffer)
                    {
                        glGenRenderbuffers(1, &depthBufferId);
                        glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
                        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                              static_cast<GLsizei>(uploadData.size.width),
                                              static_cast<GLsizei>(uploadData.size.height));
                        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);
                    }

                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureOGL->getTextureId(), 0);

#if OUZEL_SUPPORTS_OPENGL // TODO: fix this
                    //GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
                    //glDrawBuffers(1, drawBuffers);
                    glDrawBuffer(GL_COLOR_ATTACHMENT0);
#endif

                    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    {
                        Log(Log::Level::ERR) << "Failed to create frame buffer";
                        return false;
                    }
                }

                frameBufferClearColor[0] = uploadData.clearColor.getR();
                frameBufferClearColor[1] = uploadData.clearColor.getG();
                frameBufferClearColor[2] = uploadData.clearColor.getB();
                frameBufferClearColor[3] = uploadData.clearColor.getA();

                uploadData.dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
