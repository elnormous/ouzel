// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_IOS && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLIOS.hpp"
#include "core/Window.hpp"
#include "core/ios/WindowResourceIOS.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLIOS::RenderDeviceOGLIOS():
            displayLink(*this)
        {
        }

        RenderDeviceOGLIOS::~RenderDeviceOGLIOS()
        {
            displayLink.stop();
            flushCommands();

            if (msaaColorRenderBufferId) glDeleteRenderbuffersProc(1, &msaaColorRenderBufferId);
            if (msaaFrameBufferId) glDeleteFramebuffersProc(1, &msaaFrameBufferId);
            if (resolveColorRenderBufferId) glDeleteRenderbuffersProc(1, &resolveColorRenderBufferId);
            if (depthRenderBufferId) glDeleteRenderbuffersProc(1, &depthRenderBufferId);
            if (resolveFrameBufferId) glDeleteFramebuffersProc(1, &resolveFrameBufferId);

            if (context)
            {
                [EAGLContext setCurrentContext:nil];
                [context release];
            }
        }

        bool RenderDeviceOGLIOS::init(Window* newWindow,
                                      const Size2& newSize,
                                      uint32_t newSampleCount,
                                      Texture::Filter newTextureFilter,
                                      uint32_t newMaxAnisotropy,
                                      bool newVerticalSync,
                                      bool newDepth,
                                      bool newDebugRenderer)
        {
            UIView* view = static_cast<WindowResourceIOS*>(newWindow->getResource())->getNativeView();

            eaglLayer = (CAEAGLLayer*)view.layer;
            eaglLayer.opaque = YES;
            eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
            eaglLayer.contentsScale = newWindow->getResource()->getContentScale();

            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

            if (context)
            {
                apiMajorVersion = 3;
                apiMinorVersion = 0;
                Log(Log::Level::INFO) << "EAGL OpenGL ES 3 context created";
            }
            else
            {
                context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

                if (context)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    Log(Log::Level::INFO) << "EAGL OpenGL ES 2 context created";
                }
                else
                {
                    Log(Log::Level::ERR) << "Failed to create EAGL context";
                    return false;
                }
            }

            if (![EAGLContext setCurrentContext:context])
            {
                Log(Log::Level::ERR) << "Failed to set current EAGL context";
                return false;
            }

            if (!RenderDeviceOGL::init(newWindow,
                                       newSize,
                                       newSampleCount,
                                       newTextureFilter,
                                       newMaxAnisotropy,
                                       newVerticalSync,
                                       newDepth,
                                       newDebugRenderer))
                return false;

            if (!createFrameBuffer())
                return false;

            displayLink.start(verticalSync);

            return true;
        }

        void RenderDeviceOGLIOS::setSize(const Size2& newSize)
        {
            RenderDeviceOGL::setSize(newSize);

            createFrameBuffer();
        }

        bool RenderDeviceOGLIOS::lockContext()
        {
            if (![EAGLContext setCurrentContext:context])
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                return false;
            }

            return true;
        }

        bool RenderDeviceOGLIOS::swapBuffers()
        {
            if (sampleCount > 1)
            {
                glBindFramebufferProc(GL_DRAW_FRAMEBUFFER_APPLE, resolveFrameBufferId); // draw to resolve frame buffer
                glBindFramebufferProc(GL_READ_FRAMEBUFFER_APPLE, msaaFrameBufferId); // read from MSAA frame buffer

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

            glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);

            [context presentRenderbuffer:GL_RENDERBUFFER];

            return true;
        }

        bool RenderDeviceOGLIOS::createFrameBuffer()
        {
            if (sampleCount > 1)
            {
                // create resolve buffer with no depth buffer
                if (!resolveFrameBufferId) glGenFramebuffersProc(1, &resolveFrameBufferId);

                if (!resolveColorRenderBufferId) glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                graphics::RenderDeviceOGL::bindFrameBuffer(resolveFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_RENDERBUFFER, resolveColorRenderBufferId);

                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatusProc(GL_FRAMEBUFFER);
                    return false;
                }

                // create MSAA frame buffer
                if (!msaaFrameBufferId) glGenFramebuffers(1, &msaaFrameBufferId);

                if (!msaaColorRenderBufferId) glGenRenderbuffersProc(1, &msaaColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, msaaColorRenderBufferId);
                glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, static_cast<GLsizei>(sampleCount), GL_RGBA8_OES, frameBufferWidth, frameBufferHeight);

                if (depth)
                {
                    if (!depthRenderBufferId) glGenRenderbuffersProc(1, &depthRenderBufferId);
                    glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, static_cast<GLsizei>(sampleCount), GL_DEPTH_COMPONENT24_OES, frameBufferWidth, frameBufferHeight);
                }

                graphics::RenderDeviceOGL::bindFrameBuffer(msaaFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorRenderBufferId);

                if (depth)
                {
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferId);
                }

                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatusProc(GL_FRAMEBUFFER);
                    return false;
                }

                frameBufferId = msaaFrameBufferId;
            }
            else
            {
                // create resolve buffer with depth buffer
                if (!resolveFrameBufferId) glGenFramebuffersProc(1, &resolveFrameBufferId);

                if (!resolveColorRenderBufferId) glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                if (depth)
                {
                    if (!depthRenderBufferId) glGenRenderbuffersProc(1, &depthRenderBufferId);
                    glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                    glRenderbufferStorageProc(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, frameBufferWidth, frameBufferHeight);
                }

                graphics::RenderDeviceOGL::bindFrameBuffer(resolveFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_RENDERBUFFER, resolveColorRenderBufferId);

                if (depth)
                {
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferId);
                }

                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    Log(Log::Level::ERR) << "Failed to create framebuffer object " << glCheckFramebufferStatusProc(GL_FRAMEBUFFER);
                    return false;
                }

                frameBufferId = resolveFrameBufferId;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
