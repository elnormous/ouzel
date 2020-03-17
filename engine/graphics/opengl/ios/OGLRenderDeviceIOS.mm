// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_OPENGL

#include "OGLRenderDeviceIOS.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/ios/NativeWindowIOS.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            namespace
            {
                void renderCallback(void* userInfo)
                {
                    try
                    {
                        RenderDeviceIOS* renderDevice = static_cast<RenderDeviceIOS*>(userInfo);
                        renderDevice->renderCallback();
                    }
                    catch (const std::exception& e)
                    {
                        engine->log(Log::Level::Error) << e.what();
                    }
                }
            }

            RenderDeviceIOS::RenderDeviceIOS(const std::function<void(const Event&)>& initCallback):
                RenderDevice(initCallback),
                displayLink(opengl::renderCallback, this)
            {
            }

            RenderDeviceIOS::~RenderDeviceIOS()
            {
                displayLink.stop();
                CommandBuffer commandBuffer;
                commandBuffer.pushCommand(std::make_unique<PresentCommand>());
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

            void RenderDeviceIOS::init(Window* newWindow,
                                       const Size2U& newSize,
                                       std::uint32_t newSampleCount,
                                       SamplerFilter newTextureFilter,
                                       std::uint32_t newMaxAnisotropy,
                                       bool newSrgb,
                                       bool newVerticalSync,
                                       bool newDepth,
                                       bool newStencil,
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
                    apiVersion = ApiVersion(3, 0);
                    engine->log(Log::Level::Info) << "EAGL OpenGL ES 3 context created";
                }
                else
                {
                    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

                    if (!context)
                        throw std::runtime_error("Failed to create EAGL context");

                    apiVersion = ApiVersion(2, 0);
                    engine->log(Log::Level::Info) << "EAGL OpenGL ES 2 context created";
                }

                if (![EAGLContext setCurrentContext:context])
                    throw std::runtime_error("Failed to set current EAGL context");

                RenderDevice::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newSrgb,
                                   newVerticalSync,
                                   newDepth,
                                   newStencil,
                                   newDebugRenderer);

                createFrameBuffer();

                displayLink.start(verticalSync);
            }

            void RenderDeviceIOS::resizeFrameBuffer()
            {
                createFrameBuffer();
            }

            void RenderDeviceIOS::present()
            {
                if (sampleCount > 1)
                {
                    glBindFramebufferProc(GL_DRAW_FRAMEBUFFER_APPLE, resolveFrameBufferId); // draw to resolve frame buffer
                    glBindFramebufferProc(GL_READ_FRAMEBUFFER_APPLE, msaaFrameBufferId); // read from MSAA frame buffer

                    GLenum error;

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to bind MSAA frame buffer");

                    if (apiVersion.major >= 3)
                        glBlitFramebufferProc(0, 0, frameBufferWidth, frameBufferHeight,
                                              0, 0, frameBufferWidth, frameBufferHeight,
                                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
                    else
                        glResolveMultisampleFramebufferAPPLEProc();

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to blit MSAA texture");

                    // reset frame buffer
                    const GLenum discard[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
                    glDiscardFramebufferEXTProc(GL_READ_FRAMEBUFFER_APPLE, 1, discard);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to discard render buffers");

                    stateCache.frameBufferId = resolveFrameBufferId;
                }

                glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);

                [context presentRenderbuffer:GL_RENDERBUFFER];
            }

            void RenderDeviceIOS::createFrameBuffer()
            {
                if (msaaColorRenderBufferId)
                {
                    glDeleteRenderbuffersProc(1, &msaaColorRenderBufferId);
                    msaaColorRenderBufferId = 0;
                }

                if (msaaFrameBufferId)
                {
                    RenderDevice::deleteFrameBuffer(msaaFrameBufferId);
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
                    RenderDevice::deleteFrameBuffer(resolveFrameBufferId);
                    resolveFrameBufferId = 0;
                }

                if (sampleCount > 1)
                {
                    // create resolve buffer with no depth buffer
                    glGenFramebuffersProc(1, &resolveFrameBufferId);
                    glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                    glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                    if (![context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer])
                        throw std::runtime_error("Failed to bind drawable object's storage to render buffer");

                    RenderDevice::bindFrameBuffer(resolveFrameBufferId);
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                                GL_COLOR_ATTACHMENT0,
                                                GL_RENDERBUFFER,
                                                resolveColorRenderBufferId);

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

                    GLenum status;
                    if ((status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                        throw std::runtime_error("Failed to create frame buffer object, status: " + std::to_string(status));

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");

                    // create MSAA frame buffer
                    glGenFramebuffersProc(1, &msaaFrameBufferId);
                    glGenRenderbuffersProc(1, &msaaColorRenderBufferId);

                    glBindRenderbufferProc(GL_RENDERBUFFER, msaaColorRenderBufferId);
                    glRenderbufferStorageMultisampleAPPLEProc(GL_RENDERBUFFER,
                                                            static_cast<GLsizei>(sampleCount),
                                                            GL_RGBA8_OES,
                                                            frameBufferWidth,
                                                            frameBufferHeight);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set color render buffer's multisample storage");

                    if (depth)
                    {
                        glGenRenderbuffersProc(1, &depthRenderBufferId);
                        glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                        glRenderbufferStorageMultisampleAPPLEProc(GL_RENDERBUFFER,
                                                                static_cast<GLsizei>(sampleCount),
                                                                stencil ? GL_DEPTH_COMPONENT24_OES : GL_DEPTH24_STENCIL8_OES,
                                                                frameBufferWidth,
                                                                frameBufferHeight);

                        if ((error = glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set depth render buffer's multisample storage");
                    }

                    RenderDevice::bindFrameBuffer(msaaFrameBufferId);
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                                GL_COLOR_ATTACHMENT0,
                                                GL_RENDERBUFFER,
                                                msaaColorRenderBufferId);

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

                    if (depth)
                    {
                        glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                                    GL_DEPTH_ATTACHMENT,
                                                    GL_RENDERBUFFER,
                                                    depthRenderBufferId);

                        if ((error = glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
                    }

                    if ((status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                        throw std::runtime_error("Failed to create frame buffer object, status: " + std::to_string(status));

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");

                    frameBufferId = msaaFrameBufferId;
                }
                else
                {
                    // create resolve buffer with depth buffer
                    glGenFramebuffersProc(1, &resolveFrameBufferId);
                    glGenRenderbuffersProc(1, &resolveColorRenderBufferId);

                    glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);
                    if (![context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer])
                        throw std::runtime_error("Failed to bind drawable object's storage to render buffer");

                    RenderDevice::bindFrameBuffer(resolveFrameBufferId);
                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                                GL_COLOR_ATTACHMENT0,
                                                GL_RENDERBUFFER,
                                                resolveColorRenderBufferId);

                    GLenum error;
                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

                    if (depth)
                    {
                        glGenRenderbuffersProc(1, &depthRenderBufferId);
                        glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                        glRenderbufferStorageProc(GL_RENDERBUFFER,
                                                stencil ? GL_DEPTH_COMPONENT24_OES : GL_DEPTH24_STENCIL8_OES,
                                                frameBufferWidth,
                                                frameBufferHeight);

                        if ((error = glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set depth render buffer's storage");

                        glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                                    GL_DEPTH_ATTACHMENT,
                                                    GL_RENDERBUFFER,
                                                    depthRenderBufferId);

                        if ((error = glGetErrorProc()) != GL_NO_ERROR)
                            throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
                    }

                    GLenum status;
                    if ((status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
                        throw std::runtime_error("Failed to create frame buffer object, status: " + std::to_string(status));

                    if ((error = glGetErrorProc()) != GL_NO_ERROR)
                        throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");

                    frameBufferId = resolveFrameBufferId;
                }
            }

            void RenderDeviceIOS::renderCallback()
            {
                if ([EAGLContext currentContext] != context &&
                    ![EAGLContext setCurrentContext:context])
                    throw std::runtime_error("Failed to set current OpenGL context");

                process();
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
