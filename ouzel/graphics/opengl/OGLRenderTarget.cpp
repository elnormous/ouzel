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
            renderDevice.glGenFramebuffersProc(1, &frameBufferId);

            GLenum error;

            if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to upload texture data");
        }

        OGLRenderTarget::~OGLRenderTarget()
        {
            if (frameBufferId)
                renderDevice.deleteFrameBuffer(frameBufferId);
        }

        void OGLRenderTarget::reload()
        {
            for (OGLTexture* colorTexture : colorTextures)
                colorTexture->reload();
            if (depthTexture) depthTexture->reload();

            renderDevice.glGenFramebuffersProc(1, &frameBufferId);
        }

        void OGLRenderTarget::addColorTexture(OGLTexture* texture)
        {
            if (texture && colorTextures.insert(texture).second)
            {
                GLenum index = static_cast<GLenum>(colorTextures.size() - 1);
                renderDevice.bindFrameBuffer(frameBufferId);

                if (texture->getTextureId())
                    renderDevice.glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->getTextureId(), 0);
                else
                    renderDevice.glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, texture->getBufferId());

                GLenum error;
                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

                GLenum status;
                if ((status = renderDevice.glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                    throw std::runtime_error("Failed to create frame buffer, status: " + std::to_string(status));

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");
            }
        }

        void OGLRenderTarget::removeColorTexture(OGLTexture* texture)
        {
            auto i = colorTextures.find(texture);

            if (i != colorTextures.end())
            {
                colorTextures.erase(i);

                GLenum index = 0;
                for (OGLTexture* colorTexture : colorTextures)
                {
                    renderDevice.glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, colorTexture->getTextureId(), 0);
                    ++index;
                }

                renderDevice.glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, 0, 0);

                GLenum error;
                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

                GLenum status;
                if ((status = renderDevice.glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                    throw std::runtime_error("Failed to create frame buffer, status: " + std::to_string(status));

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");
            }
        }

        void OGLRenderTarget::setDepthTexture(OGLTexture* texture)
        {
            depthTexture = texture;

            if (texture)
            {
                renderDevice.bindFrameBuffer(frameBufferId);

                renderDevice.glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getTextureId(), 0);
                //glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texture->getBufferId());

                GLenum error;
                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");

                GLenum status;
                if ((status = renderDevice.glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                    throw std::runtime_error("Failed to create frame buffer, status: " + std::to_string(status));

                if ((error = renderDevice.glGetErrorProc()) != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
