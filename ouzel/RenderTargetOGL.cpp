// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetOGL.h"
#include "TextureOGL.h"

namespace ouzel
{
    RenderTargetOGL::RenderTargetOGL()
    {
        
    }
    
    RenderTargetOGL::~RenderTargetOGL()
    {
        clean();
    }
    
    void RenderTargetOGL::clean()
    {
        if (_depthBufferId) glDeleteRenderbuffers(1, &_depthBufferId);
        if (_framebufferId) glDeleteFramebuffers(1, &_framebufferId);
    }
    
    bool RenderTargetOGL::init(Size2 const& size, bool depthBuffer)
    {
        if (RenderTarget::init(size, depthBuffer))
        {
            return false;
        }
        
        clean();
        
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
        
        std::shared_ptr<TextureOGL> textureOGL(new TextureOGL());
        
        if (!textureOGL->init(size, false))
        {
            return false;
        }
        
        _texture = textureOGL;
        
        glBindTexture(GL_TEXTURE_2D, textureOGL->getTextureId());
        
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
        
#ifndef OUZEL_PLATFORM_ANDROID // TODO: fix this
        //GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        //glDrawBuffers(1, drawBuffers);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
#endif
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            return false;
        }
    
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        return true;
    }
}
