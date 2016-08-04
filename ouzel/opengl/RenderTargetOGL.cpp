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
        RenderTargetOGL::RenderTargetOGL():
            dirty(true)
        {

        }

        RenderTargetOGL::~RenderTargetOGL()
        {
            std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

            if (depthBufferId)
            {
                rendererOGL->deleteResource(depthBufferId, RendererOGL::ResourceType::RenderBuffer);
            }

            if (frameBufferId)
            {
                rendererOGL->deleteResource(frameBufferId, RendererOGL::ResourceType::FrameBuffer);
            }
        }

        void RenderTargetOGL::free()
        {
            RenderTarget::free();

            std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

            if (depthBufferId)
            {
                rendererOGL->deleteResource(depthBufferId, RendererOGL::ResourceType::RenderBuffer);
                depthBufferId = 0;
            }

            if (frameBufferId)
            {
                rendererOGL->deleteResource(frameBufferId, RendererOGL::ResourceType::FrameBuffer);
                frameBufferId = 0;
            }
        }

        bool RenderTargetOGL::init(const Size2& newSize, bool depthBuffer)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            free();

            viewport = Rectangle(0.0f, 0.0f, newSize.width, newSize.height);

            std::shared_ptr<TextureOGL> textureOGL(new TextureOGL());

            if (!textureOGL->init(size, false, false, true))
            {
                return false;
            }

            textureOGL->setFlipped(true);

            texture = textureOGL;

            ready = true;

            return true;
        }

        bool RenderTargetOGL::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                if (!frameBufferId)
                {
                    glGenFramebuffers(1, &frameBufferId);

                    if (RendererOGL::checkOpenGLError())
                    {
                        log("Failed to create frame buffer");
                        return false;
                    }
                }

                std::shared_ptr<TextureOGL> textureOGL = std::static_pointer_cast<TextureOGL>(texture);

                if (textureOGL->getTextureId())
                {
                    RendererOGL::bindFrameBuffer(frameBufferId);
                    RendererOGL::bindTexture(textureOGL->getTextureId(), 0);

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                                 static_cast<GLsizei>(size.width),
                                 static_cast<GLsizei>(size.height),
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                    if (depthBuffer)
                    {
                        glGenRenderbuffers(1, &depthBufferId);
                        glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
                        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                              static_cast<GLsizei>(size.width),
                                              static_cast<GLsizei>(size.height));
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
                        log("Failed to create frame buffer");
                        return false;
                    }
                }

                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
