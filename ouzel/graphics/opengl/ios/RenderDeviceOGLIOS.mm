// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_IOS && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLIOS.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/ios/NativeWindowIOS.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static void renderCallback(void* userInfo)
{
    try
    {
        ouzel::graphics::RenderDeviceOGLIOS* renderDevice = static_cast<ouzel::graphics::RenderDeviceOGLIOS*>(userInfo);
        renderDevice->renderCallback();
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
    }
}

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLIOS::RenderDeviceOGLIOS(const std::function<void(const Event&)>& initCallback):
            RenderDeviceOGL(initCallback),
            displayLink(::renderCallback, this)
        {
        }

        RenderDeviceOGLIOS::~RenderDeviceOGLIOS()
        {
            displayLink.stop();
            CommandBuffer commandBuffer;
            commandBuffer.commands.push(std::unique_ptr<Command>(new PresentCommand()));
            submitCommandBuffer(std::move(commandBuffer));

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

        void RenderDeviceOGLIOS::init(Window* newWindow,
                                      const Size2& newSize,
                                      uint32_t newSampleCount,
                                      Texture::Filter newTextureFilter,
                                      uint32_t newMaxAnisotropy,
                                      bool newVerticalSync,
                                      bool newDepth,
                                      bool newDebugRenderer)
        {
            UIView* view = static_cast<NativeWindowIOS*>(newWindow->getNativeWindow())->getNativeView();

            eaglLayer = (CAEAGLLayer*)view.layer;
            eaglLayer.opaque = YES;
            eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                            [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
            eaglLayer.contentsScale = newWindow->getNativeWindow()->getContentScale();

            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

            if (context)
            {
                apiMajorVersion = 3;
                apiMinorVersion = 0;
                engine->log(Log::Level::INFO) << "EAGL OpenGL ES 3 context created";
            }
            else
            {
                context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

                if (context)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    engine->log(Log::Level::INFO) << "EAGL OpenGL ES 2 context created";
                }
                else
                    throw SystemError("Failed to create EAGL context");
            }

            if (![EAGLContext setCurrentContext:context])
                throw SystemError("Failed to set current EAGL context");

            RenderDeviceOGL::init(newWindow,
                                  newSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newVerticalSync,
                                  newDepth,
                                  newDebugRenderer);

            createFrameBuffer();

            displayLink.start(verticalSync);
        }

        void RenderDeviceOGLIOS::setSize(const Size2& newSize)
        {
            RenderDeviceOGL::setSize(newSize);

            createFrameBuffer();
        }

        /*void RenderDeviceOGLIOS::lockContext()
        {
            if (![EAGLContext setCurrentContext:context])
                throw SystemError("Failed to set current OpenGL context");
        }*/

        void RenderDeviceOGLIOS::present()
        {
            if (sampleCount > 1)
            {
                glBindFramebufferProc(GL_DRAW_FRAMEBUFFER_APPLE, resolveFrameBufferId); // draw to resolve frame buffer
                glBindFramebufferProc(GL_READ_FRAMEBUFFER_APPLE, msaaFrameBufferId); // read from MSAA frame buffer

                GLenum error;

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to bind MSAA frame buffer, error: " + std::to_string(error));

                if (apiMajorVersion >= 3)
                    glBlitFramebufferProc(0, 0, frameBufferWidth, frameBufferHeight,
                                          0, 0, frameBufferWidth, frameBufferHeight,
                                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
                else
                    glResolveMultisampleFramebufferAPPLEProc();

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to blit MSAA texture, error: " + std::to_string(error));

                // reset frame buffer
                const GLenum discard[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
                glDiscardFramebufferEXTProc(GL_READ_FRAMEBUFFER_APPLE, 1, discard);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to discard render buffers, error: " + std::to_string(error));

                stateCache.frameBufferId = resolveFrameBufferId;
            }

            glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);

            [context presentRenderbuffer:GL_RENDERBUFFER];
        }

        void RenderDeviceOGLIOS::createFrameBuffer()
        {
            if (msaaColorRenderBufferId)
            {
                glDeleteRenderbuffersProc(1, &msaaColorRenderBufferId);
                msaaColorRenderBufferId = 0;
            }

            if (msaaFrameBufferId)
            {
                RenderDeviceOGL::deleteFrameBuffer(msaaFrameBufferId);
                msaaFrameBufferId = 0;
            }

            if (resolveColorRenderBufferId)
            {
                glDeleteRenderbuffersProc(1, &resolveColorRenderBufferId);
                resolveColorRenderBufferId = 0;
            }

            if (depthRenderBufferId)
            {
                glDeleteRenderbuffersProc(1, &depthRenderBufferId);
                depthRenderBufferId = 0;
            }

            if (resolveFrameBufferId)
            {
                RenderDeviceOGL::deleteFrameBuffer(resolveFrameBufferId);
                resolveFrameBufferId = 0;
            }

            if (sampleCount > 1)
            {
                // create resolve buffer with no depth buffer
                glGenFramebuffersProc(1, &resolveFrameBufferId);
                glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                if (![context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer])
                    throw SystemError("Failed to bind drawable object's storage to render buffer");

                RenderDeviceOGL::bindFrameBuffer(resolveFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_RENDERBUFFER, resolveColorRenderBufferId);

                GLenum error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to set frame buffer's color render buffer, status: " + std::to_string(error));

                GLenum status;
                if ((status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                    throw SystemError("Failed to create frame buffer object, status: " + std::to_string(status));

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to check frame buffer status, status: " + std::to_string(error));

                // create MSAA frame buffer
                glGenFramebuffersProc(1, &msaaFrameBufferId);
                glGenRenderbuffersProc(1, &msaaColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, msaaColorRenderBufferId);
                glRenderbufferStorageMultisampleAPPLEProc(GL_RENDERBUFFER, static_cast<GLsizei>(sampleCount), GL_RGBA8_OES, frameBufferWidth, frameBufferHeight);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to set color render buffer's multisample storage, status: " + std::to_string(error));

                if (depth)
                {
                    glGenRenderbuffersProc(1, &depthRenderBufferId);
                    glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                    glRenderbufferStorageMultisampleAPPLEProc(GL_RENDERBUFFER, static_cast<GLsizei>(sampleCount), GL_DEPTH_COMPONENT24_OES, frameBufferWidth, frameBufferHeight);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw DataError("Failed to set depth render buffer's multisample storage, error: " + std::to_string(error));
                }

                RenderDeviceOGL::bindFrameBuffer(msaaFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaColorRenderBufferId);

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to set frame buffer's color render buffer, status: " + std::to_string(error));

                if (depth)
                {
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferId);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw SystemError("Failed to set frame buffer's depth render buffer, status: " + std::to_string(error));
                }

                if ((status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                    throw SystemError("Failed to create frame buffer object, status: " + std::to_string(status));

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw SystemError("Failed to check frame buffer status, status: " + std::to_string(error));
                
                frameBufferId = msaaFrameBufferId;
            }
            else
            {
                // create resolve buffer with depth buffer
                glGenFramebuffersProc(1, &resolveFrameBufferId);
                glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                if (![context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer])
                    throw SystemError("Failed to bind drawable object's storage to render buffer");

                RenderDeviceOGL::bindFrameBuffer(resolveFrameBufferId);
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_RENDERBUFFER, resolveColorRenderBufferId);

                GLenum error;
                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw DataError("Failed to set frame buffer's color render buffer, error: " + std::to_string(error));

                if (depth)
                {
                    glGenRenderbuffersProc(1, &depthRenderBufferId);
                    glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                    glRenderbufferStorageProc(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, frameBufferWidth, frameBufferHeight);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw DataError("Failed to set depth render buffer's storage, error: " + std::to_string(error));

                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferId);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw DataError("Failed to set frame buffer's depth render buffer, error: " + std::to_string(error));
                }

                GLenum status;
                if ((status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                    throw SystemError("Failed to create frame buffer object, status: " + std::to_string(status));

                if ((error = glGetErrorProc()) != GL_NO_ERROR)
                    throw DataError("Failed to check frame buffer status, error: " + std::to_string(error));

                frameBufferId = resolveFrameBufferId;
            }
        }

        void RenderDeviceOGLIOS::renderCallback()
        {
            if (![EAGLContext setCurrentContext:context])
                throw SystemError("Failed to set current OpenGL context");

            process();
        }
    } // namespace graphics
} // namespace ouzel

#endif
