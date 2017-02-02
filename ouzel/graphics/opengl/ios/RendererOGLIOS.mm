// Copyright (C) 2017 Elviss Strazdins
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
                glDeleteFramebuffersProc(1, &resolveFrameBufferId);
            }

            if (resolveColorRenderBufferId)
            {
                glDeleteRenderbuffersProc(1, &resolveColorRenderBufferId);
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

        bool RendererOGLIOS::init(Window* newWindow,
                                  const Size2& newSize,
                                  uint32_t newSampleCount,
                                  Texture::Filter newTextureFilter,
                                  PixelFormat newBackBufferFormat,
                                  bool newVerticalSync,
                                  bool newDepth)
        {
            UIView* view = static_cast<WindowIOS*>(newWindow)->getNativeView();

            eaglLayer = (CAEAGLLayer*)view.layer;
            eaglLayer.opaque = YES;
            eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
            eaglLayer.contentsScale = newWindow->getContentScale();

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

            if (!RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth))
            {
                return false;
            }

            Size2 backBufferSize = Size2(static_cast<float>(frameBufferWidth),
                                         static_cast<float>(frameBufferHeight));

            newWindow->setSize(backBufferSize / window->getContentScale());

            return true;
        }

        bool RendererOGLIOS::lockContext()
        {
            if (![EAGLContext setCurrentContext:context])
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                return false;
            }

            return true;
        }

        bool RendererOGLIOS::swapBuffers()
        {
            if (sampleCount > 1)
            {
                glBindFramebufferProc(GL_DRAW_FRAMEBUFFER_APPLE, resolveFrameBufferId); // draw to resolve frame buffer
                glBindFramebufferProc(GL_READ_FRAMEBUFFER_APPLE, frameBufferId); // read from FBO

                if (checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to bind MSAA frame buffer";
                    return false;
                }

                if (apiMajorVersion >= 3)
                {
                    glBlitFramebufferProc(0, 0, frameBufferWidth, frameBufferHeight,
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
                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
            }
            else
            {
                glBindRenderbufferProc(GL_RENDERBUFFER, colorRenderBufferId);
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
                    glGenFramebuffersProc(1, &resolveFrameBufferId);
                }

                if (!resolveColorRenderBufferId)
                {
                    glGenRenderbuffersProc(1, &resolveColorRenderBufferId);
                }

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth);
                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight);

                graphics::RendererOGL::bindFrameBuffer(resolveFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_RENDERBUFFER, resolveColorRenderBufferId);

                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatusProc(GL_FRAMEBUFFER);
                    return false;
                }

                systemFrameBufferId = resolveFrameBufferId;
            }
            else
            {
                if (!frameBufferId) glGenFramebuffersProc(1, &frameBufferId);

                if (!colorRenderBufferId) glGenRenderbuffersProc(1, &colorRenderBufferId);
                glBindRenderbufferProc(GL_RENDERBUFFER, colorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &frameBufferWidth);
                glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &frameBufferHeight);

                if (depth)
                {
                    GLuint depthFormat = GL_DEPTH_COMPONENT24_OES;

                    if (!depthFormat)
                    {
                        Log(Log::Level::ERR) << "Unsupported depth buffer format";
                        return false;
                    }

                    if (!depthRenderBufferId) glGenRenderbuffersProc(1, &depthRenderBufferId);
                    glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                    glRenderbufferStorageProc(GL_RENDERBUFFER, depthFormat, frameBufferWidth, frameBufferHeight);
                }

                graphics::RendererOGL::bindFrameBuffer(frameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBufferId);

                if (depth)
                {
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferId);
                }

                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatusProc(GL_FRAMEBUFFER);
                    return false;
                }

                systemFrameBufferId = frameBufferId;
            }

            return RendererOGL::createFrameBuffer();
        }
    } // namespace graphics
} // namespace ouzel
