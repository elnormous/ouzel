// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#include <array>
#include <utility>
#include "OGLRenderDeviceTVOS.hpp"
#include "../../../core/Engine.hpp"
#include "../../../core/Window.hpp"
#include "../../../core/tvos/NativeWindowTVOS.hpp"
#include "../../../platform/foundation/AutoreleasePool.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::opengl::tvos
{
    namespace
    {
        void renderCallback(void* userInfo)
        {
            try
            {
                const auto renderDevice = static_cast<RenderDevice*>(userInfo);
                renderDevice->renderCallback();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }
        }
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice{settings, initWindow, initCallback},
        displayLink{tvos::renderCallback, this}
    {
        embedded = true;

        const auto view = static_cast<core::tvos::NativeWindow*>(window.getNativeWindow())->getNativeView();

        eaglLayer = (CAEAGLLayer*)view.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        eaglLayer.contentsScale = window.getNativeWindow()->getContentScale();

        shareGroup = [[EAGLSharegroup alloc] init];

        constexpr std::array<std::pair<EAGLRenderingAPI, std::uint16_t>, 2> openGlVersions = {
            std::pair(kEAGLRenderingAPIOpenGLES3, 3),
            std::pair(kEAGLRenderingAPIOpenGLES2, 2)
        };

        for (const auto& [eaglOpenGlVersion, openGlVersion] : openGlVersions)
        {
            context = [[EAGLContext alloc] initWithAPI:eaglOpenGlVersion
                                            sharegroup:shareGroup];

            if (context)
            {
                apiVersion = ApiVersion{openGlVersion, 0};
                logger.log(Log::Level::info) << "EAGL OpenGL ES " << apiVersion << " context created";
                break;
            }
        }

        if (!context)
            throw std::runtime_error("Failed to create EAGL context");

        if (![EAGLContext setCurrentContext:context])
            throw std::runtime_error("Failed to set current EAGL context");

        init(static_cast<GLsizei>(window.getResolution().v[0]),
             static_cast<GLsizei>(window.getResolution().v[1]));

        createFrameBuffer();

        running = true;
        renderThread = thread::Thread{&RenderDevice::renderMain, this};
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        std::unique_lock lock{runLoopMutex};
        runLoop.stop();
        lock.unlock();
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

        if (shareGroup) [shareGroup release];
    }

    void RenderDevice::resizeFrameBuffer()
    {
        createFrameBuffer();
    }

    void RenderDevice::present()
    {
        if (sampleCount > 1)
        {
            glBindFramebufferProc(GL_DRAW_FRAMEBUFFER_APPLE, resolveFrameBufferId); // draw to resolve frame buffer
            glBindFramebufferProc(GL_READ_FRAMEBUFFER_APPLE, frameBufferId); // read from FBO

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to bind MSAA frame buffer");

            if (apiVersion.v[0] >= 3)
                glBlitFramebufferProc(0, 0, frameBufferWidth, frameBufferHeight,
                                      0, 0, frameBufferWidth, frameBufferHeight,
                                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
            else
                glResolveMultisampleFramebufferAPPLEProc();

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to blit MSAA texture");

            // reset frame buffer
            const GLenum discard[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
            glDiscardFramebufferEXTProc(GL_READ_FRAMEBUFFER_APPLE, 1, discard);

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to discard render buffers");

            stateCache.frameBufferId = resolveFrameBufferId;
        }

        glBindRenderbufferProc(GL_RENDERBUFFER, resolveColorRenderBufferId);

        [context presentRenderbuffer:GL_RENDERBUFFER];
    }

    void RenderDevice::createFrameBuffer()
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

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

            if (const auto status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Failed to create frame buffer object, status: " + std::to_string(status));

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");

            // create MSAA frame buffer
            glGenFramebuffersProc(1, &msaaFrameBufferId);
            glGenRenderbuffersProc(1, &msaaColorRenderBufferId);

            glBindRenderbufferProc(GL_RENDERBUFFER, msaaColorRenderBufferId);
            glRenderbufferStorageMultisampleProc(GL_RENDERBUFFER,
                                                 static_cast<GLsizei>(sampleCount),
                                                 GL_RGBA8_OES,
                                                 frameBufferWidth,
                                                 frameBufferHeight);

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set color render buffer's multisample storage");

            if (depth)
            {
                glGenRenderbuffersProc(1, &depthRenderBufferId);
                glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                glRenderbufferStorageMultisampleProc(GL_RENDERBUFFER,
                                                     static_cast<GLsizei>(sampleCount),
                                                     stencil ? GL_DEPTH_COMPONENT24_OES : GL_DEPTH24_STENCIL8_OES,
                                                     frameBufferWidth,
                                                     frameBufferHeight);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set depth render buffer's multisample storage");
            }

            RenderDevice::bindFrameBuffer(msaaFrameBufferId);
            glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                          GL_COLOR_ATTACHMENT0,
                                          GL_RENDERBUFFER,
                                          msaaColorRenderBufferId);

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

            if (depth)
            {
                glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                              GL_DEPTH_ATTACHMENT,
                                              GL_RENDERBUFFER,
                                              depthRenderBufferId);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
            }

            if (const auto status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Failed to create frame buffer object, status: " + std::to_string(status));

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
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

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's color render buffer");

            if (depth)
            {
                glGenRenderbuffersProc(1, &depthRenderBufferId);
                glBindRenderbufferProc(GL_RENDERBUFFER, depthRenderBufferId);
                glRenderbufferStorageProc(GL_RENDERBUFFER,
                                          stencil ? GL_DEPTH_COMPONENT24_OES : GL_DEPTH24_STENCIL8_OES,
                                          frameBufferWidth,
                                          frameBufferHeight);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set depth render buffer's storage");

                glFramebufferRenderbufferProc(GL_FRAMEBUFFER,
                                              GL_DEPTH_ATTACHMENT,
                                              GL_RENDERBUFFER,
                                              depthRenderBufferId);

                if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                    throw std::system_error(makeErrorCode(error), "Failed to set frame buffer's depth render buffer");
            }

            if (const auto status = glCheckFramebufferStatusProc(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Failed to create frame buffer object, status: " + std::to_string(status));

            if (const auto error = glGetErrorProc(); error != GL_NO_ERROR)
                throw std::system_error(makeErrorCode(error), "Failed to check frame buffer status");

            frameBufferId = resolveFrameBufferId;
        }
    }

    void RenderDevice::renderMain()
    {
        thread::setCurrentThreadName("Render");

        if (verticalSync)
        {
            std::unique_lock lock{runLoopMutex};
            runLoop = platform::foundation::RunLoop{};
            lock.unlock();

            displayLink.addToRunLoop(runLoop);
            runLoop.run();
        }
        else while (running)
            renderCallback();
    }

    void RenderDevice::renderCallback()
    {
        platform::foundation::AutoreleasePool autoreleasePool;
        if ([EAGLContext currentContext] != context &&
            ![EAGLContext setCurrentContext:context])
            throw std::runtime_error("Failed to set current OpenGL context");

        process();
    }
}

#endif
