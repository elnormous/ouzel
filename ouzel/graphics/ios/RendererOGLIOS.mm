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

        bool RendererOGLIOS::init(Window* newWindow,
                                  const Size2& newSize,
                                  uint32_t newSampleCount,
                                  TextureFilter newTextureFilter,
                                  PixelFormat newBackBufferFormat,
                                  bool newVerticalSync,
                                  uint32_t newDepthBits)
        {
            free();

            UIView* view = static_cast<WindowIOS*>(newWindow)->getNativeView();

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

            if (!createRenderBuffer())
            {
                return false;
            }

            Size2 backBufferSize = Size2(static_cast<float>(frameBufferWidth),
                                         static_cast<float>(frameBufferHeight));

            newWindow->setSize(backBufferSize);

            return RendererOGL::init(newWindow, backBufferSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepthBits);
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

        bool RendererOGLIOS::createRenderBuffer()
        {
            if (!RendererOGL::createRenderBuffer())
            {
                return false;
            }

            if (!frameBufferId)
            {
                glGenFramebuffers(1, &frameBufferId);
            }

            if (colorRenderBuffer)
            {
                glDeleteRenderbuffers(1, &colorRenderBuffer);
                colorRenderBuffer = 0;
            }

            glGenRenderbuffers(1, &colorRenderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
            [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight);

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
    } // namespace graphics
} // namespace ouzel
