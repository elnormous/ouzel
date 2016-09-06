// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLIOS.h"
#include "core/Engine.h"
#include "WindowIOS.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLIOS::~RendererOGLIOS()
        {
            if (context)
            {
                if ([EAGLContext currentContext] == context)
                {
                    [EAGLContext setCurrentContext:nil];
                }
                [context release];
            }

            if (frameBufferId)
            {
                glDeleteFramebuffers(1, &frameBufferId);
            }

            if (colorRenderBuffer)
            {
                glDeleteRenderbuffers(1, &colorRenderBuffer);
            }
        }

        void RendererOGLIOS::free()
        {
            if (context)
            {
                if ([EAGLContext currentContext] == context)
                {
                    [EAGLContext setCurrentContext:nil];
                }
                [context release];
            }

            if (frameBufferId)
            {
                glDeleteFramebuffers(1, &frameBufferId);
                frameBufferId = 0;
            }

            if (colorRenderBuffer)
            {
                glDeleteRenderbuffers(1, &colorRenderBuffer);
                colorRenderBuffer = 0;
            }
        }

        bool RendererOGLIOS::init(const WindowPtr& window,
                                  uint32_t newSampleCount,
                                  TextureFiltering newTextureFiltering,
                                  bool newVerticalSync)
        {
            free();

            UIView* view = std::static_pointer_cast<WindowIOS>(window)->getNativeView();

            eaglLayer = (CAEAGLLayer*)view.layer;
            eaglLayer.opaque = YES;
            eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

            if (context)
            {
                apiVersion = 3;
                log(LOG_LEVEL_INFO, "Using OpenGL ES 3");
            }
            else
            {
                log(LOG_LEVEL_INFO, "Failed to create OpenGL ES 3 rendering context");

                context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

                if (context)
                {
                    apiVersion = 2;
                    log(LOG_LEVEL_INFO, "Using OpenGL ES 2");
                }
                else
                {
                    log(LOG_LEVEL_ERROR, "Failed to initialize OpenGL ES 2 rendering context");
                    return false;
                }
            }

            if (![EAGLContext setCurrentContext:context])
            {
                log(LOG_LEVEL_ERROR, "Failed to set current OpenGL rendering context");
                return false;
            }

            glGenFramebuffers(1, &frameBufferId);

            Size2 renderBufferSize;
            if (!createRenderBuffer(renderBufferSize))
            {
                return false;
            }

            window->setSize(renderBufferSize);

            return RendererOGL::init(window, newSampleCount, newTextureFiltering, newVerticalSync);
        }

        void RendererOGLIOS::setSize(const Size2& newSize)
        {
            RendererOGL::setSize(newSize);

            Size2 renderBufferSize;

            if (!createRenderBuffer(renderBufferSize))
            {
                return;
            }

            if (renderBufferSize != size)
            {
                sharedEngine->getWindow()->setSize(renderBufferSize);
            }
        }

        bool RendererOGLIOS::createRenderBuffer(Size2& renderBufferSize)
        {
            if (colorRenderBuffer)
            {
                glDeleteRenderbuffers(1, &colorRenderBuffer);
                colorRenderBuffer = 0;
            }

            glGenRenderbuffers(1, &colorRenderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
            [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

            GLint frameBufferWidth;
            GLint frameBufferHeight;
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight);

            renderBufferSize.width = static_cast<float>(frameBufferWidth);
            renderBufferSize.height = static_cast<float>(frameBufferHeight);

            graphics::RendererOGL::bindFrameBuffer(frameBufferId);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                      GL_RENDERBUFFER, colorRenderBuffer);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                log(LOG_LEVEL_ERROR, "Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
                return false;
            }

            return true;
        }

        bool RendererOGLIOS::present()
        {
            if (![EAGLContext setCurrentContext:context])
            {
                log(LOG_LEVEL_ERROR, "Failed to set current OpenGL context");
            }

            if (!RendererOGL::present())
            {
                return false;
            }

            [context presentRenderbuffer:GL_RENDERBUFFER];

            return true;
        }
    } // namespace graphics
} // namespace ouzel
