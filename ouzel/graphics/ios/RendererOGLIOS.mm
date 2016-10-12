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

        bool RendererOGLIOS::init(const WindowPtr& newWindow,
                                  uint32_t newSampleCount,
                                  TextureFilter newTextureFilter,
                                  PixelFormat newBackBufferFormat,
                                  bool newVerticalSync)
        {
            free();

            UIView* view = std::static_pointer_cast<WindowIOS>(newWindow)->getNativeView();

            eaglLayer = (CAEAGLLayer*)view.layer;
            eaglLayer.opaque = YES;
            eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

            if (context)
            {
                apiMajorVersion = 3;
                apiMinorVersion = 0;
                Log(Log::Level::INFO) << "Using OpenGL ES 3";
            }
            else
            {
                Log(Log::Level::INFO) << "Failed to create OpenGL ES 3 rendering context";

                context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

                if (context)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    Log(Log::Level::INFO) << "Using OpenGL ES 2";
                }
                else
                {
                    Log(Log::Level::ERR) << "Failed to initialize OpenGL ES 2 rendering context";
                    return false;
                }
            }

            if (![EAGLContext setCurrentContext:context])
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL rendering context";
                return false;
            }

            glGenFramebuffers(1, &frameBufferId);

            Size2 renderBufferSize;
            if (!createRenderBuffer(renderBufferSize))
            {
                return false;
            }

            newWindow->setSize(renderBufferSize);

            return RendererOGL::init(newWindow, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync);
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
                window->setSize(renderBufferSize);
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
                Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatus(GL_FRAMEBUFFER);
                return false;
            }

            return true;
        }

        bool RendererOGLIOS::present()
        {
            if (![EAGLContext setCurrentContext:context])
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
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
