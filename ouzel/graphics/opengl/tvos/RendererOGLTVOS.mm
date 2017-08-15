// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_SUPPORTS_OPENGL

#import <QuartzCore/QuartzCore.h>
#include "RendererOGLTVOS.hpp"
#include "core/tvos/WindowTVOS.hpp"
#include "utils/Log.hpp"

@interface DisplayLinkHandler: NSObject
{
    ouzel::graphics::Renderer* renderer;
    NSConditionLock* runLock;
    NSRunLoop* runLoop;
    CADisplayLink* displayLink;
    NSThread* renderThread;
    bool verticalSync;
    bool running;
}

@end

@implementation DisplayLinkHandler

-(id)initWithRenderer:(ouzel::graphics::Renderer*)newRenderer andVerticalSync:(bool)newVerticalSync
{
    if (self = [super init])
    {
        renderer = newRenderer;
        verticalSync = newVerticalSync;
        running = true;

        if (verticalSync)
        {
            displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];
            if (!displayLink)
            {
                ouzel::Log(ouzel::Log::Level::ERR) << "Failed to create display link";
                return nil;
            }
            [displayLink setFrameInterval:1.0f];
        }

        runLock = [[NSConditionLock alloc] initWithCondition:0];
        renderThread = [[NSThread alloc] initWithTarget:self selector:@selector(runThread) object:nil];
        [renderThread start];
    }

    return self;
}

-(void)dealloc
{
    [runLock lockWhenCondition:1];
    [runLock unlock];
    [runLock release];
    [displayLink invalidate];
    [displayLink release];
    [renderThread release];
    [super dealloc];
}

-(void)draw:(__unused CADisplayLink*)sender
{
    @autoreleasepool
    {
        renderer->process();
    }
}

-(void)runThread
{
    [runLock lock];

    if (verticalSync)
    {
        runLoop = [NSRunLoop currentRunLoop];
        [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
        [runLoop run];
    }
    else
    {
        while (running) [self draw:nil];
    }

    [runLock unlockWithCondition:1];
}

-(void)stop
{
    [self performSelector:@selector(exit) onThread:renderThread withObject:nil waitUntilDone:NO];
}

-(void)exit
{
    running = false;
    if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
}

@end

namespace ouzel
{
    namespace graphics
    {
        RendererOGLTVOS::~RendererOGLTVOS()
        {
            if (displayLinkHandler) [displayLinkHandler stop];
            flushCommands();
            if (displayLinkHandler) [displayLinkHandler dealloc];

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

        bool RendererOGLTVOS::init(Window* newWindow,
                                   const Size2& newSize,
                                   uint32_t newSampleCount,
                                   Texture::Filter newTextureFilter,
                                   uint32_t newMaxAnisotropy,
                                   bool newVerticalSync,
                                   bool newDepth,
                                   bool newDebugRenderer)
        {
            UIView* view = static_cast<WindowTVOS*>(newWindow)->getNativeView();

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

            if (!RendererOGL::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newVerticalSync,
                                   newDepth,
                                   newDebugRenderer))
            {
                return false;
            }

            frameBufferWidth = static_cast<GLsizei>(newSize.v[0]);
            frameBufferHeight = static_cast<GLsizei>(newSize.v[1]);

            if (!createFrameBuffer())
            {
                return false;
            }

            displayLinkHandler = [[DisplayLinkHandler alloc] initWithRenderer:this];

            return true;
        }

        bool RendererOGLTVOS::lockContext()
        {
            if (![EAGLContext setCurrentContext:context])
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                return false;
            }

            return true;
        }

        bool RendererOGLTVOS::swapBuffers()
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

            glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);

            [context presentRenderbuffer:GL_RENDERBUFFER];

            return true;
        }

        bool RendererOGLTVOS::createFrameBuffer()
        {
            if (sampleCount > 1)
            {
                // create resolve buffer with no depth buffer
                if (!resolveFrameBufferId) glGenFramebuffersProc(1, &resolveFrameBufferId);

                if (!resolveColorRenderBufferId) glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

                graphics::RendererOGL::bindFrameBuffer(resolveFrameBufferId);
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

                graphics::RendererOGL::bindFrameBuffer(msaaFrameBufferId);
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

                graphics::RendererOGL::bindFrameBuffer(resolveFrameBufferId);
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

        bool RendererOGLTVOS::upload()
        {
            if (frameBufferWidth != static_cast<GLsizei>(size.v[0]) ||
                frameBufferHeight != static_cast<GLsizei>(size.v[1]))
            {
                frameBufferWidth = static_cast<GLsizei>(size.v[0]);
                frameBufferHeight = static_cast<GLsizei>(size.v[1]);

                if (!createFrameBuffer())
                {
                    return false;
                }
            }

            return RendererOGL::upload();
        }
    } // namespace graphics
} // namespace ouzel

#endif
