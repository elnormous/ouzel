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
            if (resolveFrameBufferId)
            {
                glDeleteFramebuffers(1, &resolveFrameBufferId);
            }

            if (resolveColorRenderBufferId)
            {
                glDeleteRenderbuffers(1, &resolveColorRenderBufferId);
            }
            
            if (context)
            {
                if ([EAGLContext currentContext] == context)
                {
                    [EAGLContext setCurrentContext:nil];
                }
                [context release];
            }
        }

        void RendererOGLIOS::free()
        {
            if (resolveFrameBufferId)
            {
                glDeleteFramebuffers(1, &resolveFrameBufferId);
                resolveFrameBufferId = 0;
            }

            if (resolveColorRenderBufferId)
            {
                glDeleteRenderbuffers(1, &resolveColorRenderBufferId);
                resolveColorRenderBufferId = 0;
            }

            if (context)
            {
                if ([EAGLContext currentContext] == context)
                {
                    [EAGLContext setCurrentContext:nil];
                }
                [context release];
                context = Nil;
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

            if (!RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepthBits))
            {
                return false;
            }

            Size2 backBufferSize = Size2(static_cast<float>(frameBufferWidth),
                                         static_cast<float>(frameBufferHeight));

            newWindow->setSize(backBufferSize);

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

            if (sampleCount > 1)
            {
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, resolveFrameBufferId); // draw to resolve frame buffer
                glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, frameBufferId); // read from FBO

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to bind MSAA frame buffer";
                    return false;
                }

                if (apiMajorVersion >= 3)
                {
                    glBlitFramebuffer(0, 0, frameBufferWidth, frameBufferHeight,
                                      0, 0, frameBufferWidth, frameBufferHeight,
                                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
                }
                else
                {
                    glResolveMultisampleFramebufferAPPLE();
                }

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to blit MSAA texture";
                    return false;
                }

                // reset framebuffer
                const GLenum discard[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, discard);

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to discard render buffers";
                    return false;
                }

                stateCache.frameBufferId = resolveFrameBufferId;
            }

            if (sampleCount > 1)
            {
                glBindRenderbuffer(GL_RENDERBUFFER, resolveColorRenderBufferId);
            }
            else
            {
                glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBufferId);
            }

            [context presentRenderbuffer:GL_RENDERBUFFER];

            return true;
        }

        bool RendererOGLIOS::createFrameBuffer()
        {
            if (sampleCount > 1)
            {
                // resolve buffer
                if (!resolveFrameBufferId)
                {
                    glGenFramebuffers(1, &resolveFrameBufferId);
                }

                if (!resolveColorRenderBufferId)
                {
                    glGenRenderbuffers(1, &resolveColorRenderBufferId);
                }

                glBindRenderbuffer(GL_RENDERBUFFER, resolveColorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth);
                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight);

                graphics::RendererOGL::bindFrameBuffer(resolveFrameBufferId);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                          GL_RENDERBUFFER, resolveColorRenderBufferId);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatus(GL_FRAMEBUFFER);
                    return false;
                }
            }
            else
            {
                if (!frameBufferId) glGenFramebuffers(1, &frameBufferId);

                if (!colorRenderBufferId) glGenRenderbuffers(1, &colorRenderBufferId);
                glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth);
                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight);

                if (depthBits > 0)
                {
                    GLuint depthFormat = getDepthFormat(depthBits);

                    if (!depthFormat)
                    {
                        Log(Log::Level::ERR) << "Unsupported depth buffer format";
                        return false;
                    }

                    if (!depthRenderBufferId) glGenRenderbuffers(1, &depthRenderBufferId);
                    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferId);
                    glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, frameBufferWidth, frameBufferHeight);
                }

                graphics::RendererOGL::bindFrameBuffer(frameBufferId);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBufferId);

                if (depthBits > 0)
                {
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferId);
                }

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatus(GL_FRAMEBUFFER);
                    return false;
                }
            }
            
            return RendererOGL::createFrameBuffer();
        }
    } // namespace graphics
} // namespace ouzel
