// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetOGL.h"

namespace ouzel
{
    RenderTargetOGL::RenderTargetOGL()
    {
        
    }
    
    RenderTargetOGL::~RenderTargetOGL()
    {
        
    }
    
    bool RenderTargetOGL::init(Size2 const& size)
    {
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);
        
        glBindTexture(GL_TEXTURE_2D, _textureId);
        
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, size.width, size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        
        return true;
    }
}
