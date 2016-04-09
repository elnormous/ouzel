// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetOGL.h"
#include "TextureOGL.h"
#include "Engine.h"
#include "RendererOGL.h"

namespace ouzel
{
    namespace video
    {
        RenderTargetOGL::RenderTargetOGL()
        {

        }

        RenderTargetOGL::~RenderTargetOGL()
        {
            destroy();
        }

        void RenderTargetOGL::destroy()
        {
            if (_depthBufferId) glDeleteRenderbuffers(1, &_depthBufferId);
            if (_framebufferId) glDeleteFramebuffers(1, &_framebufferId);
        }

        bool RenderTargetOGL::init(const Size2& size, bool depthBuffer)
        {
            if (RenderTarget::init(size, depthBuffer))
            {
                return false;
            }

            destroy();

            GLuint oldFrameBufferId;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&oldFrameBufferId));

            glGenFramebuffers(1, &_framebufferId);

            RendererOGL::bindFramebuffer(_framebufferId);

            std::shared_ptr<TextureOGL> textureOGL(new TextureOGL());

            if (!textureOGL->init(size, false))
            {
                return false;
            }

            _texture = textureOGL;

            RendererOGL::bindTexture(textureOGL->getTextureId(), 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                         static_cast<GLsizei>(_size.width),
                         static_cast<GLsizei>(_size.height),
                         0, GL_RGB, GL_UNSIGNED_BYTE, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            if (depthBuffer)
            {
                glGenRenderbuffers(1, &_depthBufferId);
                glBindRenderbuffer(GL_RENDERBUFFER, _depthBufferId);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                      static_cast<GLsizei>(_size.width),
                                      static_cast<GLsizei>(_size.height));
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBufferId);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureOGL->getTextureId(), 0);

    #ifdef OUZEL_SUPPORTS_OPENGL // TODO: fix this
            //GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            //glDrawBuffers(1, drawBuffers);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
    #endif

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                return false;
            }

            return true;
        }
    } // namespace video
} // namespace ouzel
