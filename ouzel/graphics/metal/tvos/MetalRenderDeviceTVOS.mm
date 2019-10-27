// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if TARGET_OS_TV && OUZEL_COMPILE_METAL

#include "MetalRenderDeviceTVOS.hpp"
#include "MetalView.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace metal
        {
            namespace
            {
                void renderCallback(void* userInfo)
                {
                    try
                    {
                        RenderDeviceTVOS* renderDevice = static_cast<RenderDeviceTVOS*>(userInfo);
                        renderDevice->renderCallback();
                    }
                    catch (const std::exception& e)
                    {
                        engine->log(Log::Level::Error) << e.what();
                    }
                }
            }

            RenderDeviceTVOS::RenderDeviceTVOS(const std::function<void(const Event&)>& initCallback):
                RenderDevice(initCallback),
                displayLink(metal::renderCallback, this)
            {
            }

            RenderDeviceTVOS::~RenderDeviceTVOS()
            {
                displayLink.stop();
                CommandBuffer commandBuffer;
                commandBuffer.pushCommand(std::make_unique<PresentCommand>());
                submitCommandBuffer(std::move(commandBuffer));
            }

            void RenderDeviceTVOS::init(Window* newWindow,
                                        const Size2U& newSize,
                                        uint32_t newSampleCount,
                                        SamplerFilter newTextureFilter,
                                        uint32_t newMaxAnisotropy,
                                        bool newSrgb,
                                        bool newVerticalSync,
                                        bool newDepth,
                                        bool newStencil,
                                        bool newDebugRenderer)
            {
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

                MetalView* view = (MetalView*)static_cast<NativeWindowTVOS*>(newWindow->getNativeWindow())->getNativeView();

                metalLayer = (CAMetalLayer*)view.layer;
                metalLayer.device = device;
                const CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
                metalLayer.drawableSize = drawableSize;

                colorFormat = metalLayer.pixelFormat;

                displayLink.start(verticalSync);
            }

            void RenderDeviceTVOS::renderCallback()
            {
                process();
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
