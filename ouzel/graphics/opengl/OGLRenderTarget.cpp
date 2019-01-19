// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include <stdexcept>
#include "OGLRenderTarget.hpp"
#include "OGLRenderDevice.hpp"
#include "OGLTexture.hpp"

namespace ouzel
{
    namespace graphics
    {
        OGLRenderTarget::OGLRenderTarget(OGLRenderDevice& renderDeviceOGL):
            OGLRenderResource(renderDeviceOGL)
        {
            glGenFramebuffersProc(1, &frameBufferId);

            GLenum error;

            if ((error = glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to upload texture data");

            clearMask = GL_COLOR_BUFFER_BIT;
        }

        OGLRenderTarget::~OGLRenderTarget()
        {
            if (frameBufferId)
                renderDevice.deleteFrameBuffer(frameBufferId);
        }

        void OGLRenderTarget::reload()
        {
            if (colorTexture) colorTexture->reload();
            if (depthTexture) depthTexture->reload();

            glGenFramebuffersProc(1, &frameBufferId);
        }

        void OGLRenderTarget::setColorTexture(OGLTexture* texture)
        {
            colorTexture = texture;

            if (texture)
            {
                glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTextureId(), 0);
                //glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, texture->getBufferId());

                GLenum error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
            }
        }

        void OGLRenderTarget::setDepthTexture(OGLTexture* texture)
        {
            depthTexture = texture;

            if (texture)
            {
                glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getTextureId(), 0);
                //glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texture->getBufferId());

                GLenum error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
