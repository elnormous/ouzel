// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_METAL

#include "MetalRenderDeviceIOS.hpp"
#include "MetalView.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/ios/NativeWindowIOS.hpp"
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
                displayLink(metal::renderCallback, this)
            {
            }

            RenderDeviceIOS::~RenderDeviceIOS()
            {
                displayLink.stop();
                CommandBuffer commandBuffer;
                commandBuffer.pushCommand(std::make_unique<PresentCommand>());
                submitCommandBuffer(std::move(commandBuffer));
            }

            void RenderDeviceIOS::init(Window* newWindow,
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

                NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(newWindow->getNativeWindow());
                MetalView* view = (MetalView*)windowIOS->getNativeView();

                metalLayer = (CAMetalLayer*)view.layer;
                metalLayer.device = device;
                const CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
                metalLayer.drawableSize = drawableSize;

                colorFormat = metalLayer.pixelFormat;

                displayLink.start(verticalSync);
            }

            void RenderDeviceIOS::renderCallback()
            {
                process();
            }
        } // namespace metal
    } // namespace graphics
} // namespace ouzel

#endif
