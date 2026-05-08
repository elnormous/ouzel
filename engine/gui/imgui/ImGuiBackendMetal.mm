#include "../../core/Setup.h"

#if FLUXION_COMPILE_METAL

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include "ImGuiBackend.hpp"
#include "ImGuiInputBridge.hpp"
#include "../../core/Engine.hpp"
#include "../../graphics/Graphics.hpp"
#include "../../graphics/metal/MetalRenderDevice.hpp"
#include "imgui.h"
#include "backends/imgui_impl_metal.h"

namespace fluxion::gui::imgui
{
    namespace
    {
        class MetalBackend final : public Backend
        {
        public:
            MetalBackend(core::Engine& eng,
                         graphics::metal::RenderDevice& metalDevice):
                imguiContext{ImGui::CreateContext()},
                deviceRef{&metalDevice},
                graphicsRef{&eng.getGraphics()},
                inputBridge{eng.getEventDispatcher()},
                lastFrameTime{Clock::now()}
            {
                ImGui::SetCurrentContext(imguiContext);
                ImGuiIO& io = ImGui::GetIO();
                io.IniFilename = nullptr;
                io.LogFilename = nullptr;
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                io.BackendPlatformName = "fluxion-events";
                io.BackendRendererName = "imgui_impl_metal";

                ImGui::StyleColorsDark();

                id<MTLDevice> mtlDevice = (__bridge id<MTLDevice>)metalDevice.getDevice().get();
                if (!ImGui_ImplMetal_Init(mtlDevice))
                    throw std::runtime_error{"ImGui_ImplMetal_Init failed"};

                deviceRef->setPreSwapHook([this]() { renderFrame(); });
            }

            ~MetalBackend() override
            {
                if (deviceRef) deviceRef->setPreSwapHook({});

                // ImGui_ImplMetal_Shutdown releases Metal resources. If any
                // frame was rendered, this must run on the render thread so
                // it sees the same MTLDevice/MTLCommandQueue state. If no
                // frame was rendered (Init only allocated host-side state),
                // running on the main thread is safe too.
                if (deviceRef && rendererInitialized)
                {
                    std::promise<void> done;
                    auto future = done.get_future();
                    deviceRef->executeOnRenderThread([this, &done]() {
                        ImGui::SetCurrentContext(imguiContext);
                        ImGui_ImplMetal_Shutdown();
                        done.set_value();
                    });
                    future.wait();
                }
                else
                {
                    ImGui::SetCurrentContext(imguiContext);
                    ImGui_ImplMetal_Shutdown();
                }

                ImGui::DestroyContext(imguiContext);
            }

            void setDrawCallback(DrawCallback cb) override
            {
                std::lock_guard lock{callbackMutex};
                drawCallback = std::move(cb);
            }

            void setEnabled(bool e) noexcept override { enabled.store(e, std::memory_order_release); }
            bool isEnabled() const noexcept override { return enabled.load(std::memory_order_acquire); }

            bool wantsKeyboard() const noexcept override { return inputBridge.wantsKeyboard(); }
            bool wantsMouse() const noexcept override { return inputBridge.wantsMouse(); }

        private:
            using Clock = std::chrono::steady_clock;

            void renderFrame()
            {
                if (!enabled.load(std::memory_order_acquire)) return;

                ImGui::SetCurrentContext(imguiContext);
                rendererInitialized = true;

                id<MTLCommandBuffer> commandBuffer =
                    (__bridge id<MTLCommandBuffer>)deviceRef->getCurrentCommandBuffer();
                id<MTLTexture> drawableTexture =
                    (__bridge id<MTLTexture>)deviceRef->getCurrentDrawableTexture();
                if (!commandBuffer || !drawableTexture) return;

                MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
                passDesc.colorAttachments[0].texture = drawableTexture;
                passDesc.colorAttachments[0].loadAction = MTLLoadActionLoad;
                passDesc.colorAttachments[0].storeAction = MTLStoreActionStore;

                const auto now = Clock::now();
                const float dt = std::chrono::duration<float>{now - lastFrameTime}.count();
                lastFrameTime = now;

                inputBridge.newFrame(graphicsRef->getSize(), dt);

                ImGui_ImplMetal_NewFrame(passDesc);
                ImGui::NewFrame();
                inputBridge.snapshotWantCapture(ImGui::GetIO());

                DrawCallback cb;
                {
                    std::lock_guard lock{callbackMutex};
                    cb = drawCallback;
                }
                if (cb) cb();

                ImGui::Render();

                id<MTLRenderCommandEncoder> encoder =
                    [commandBuffer renderCommandEncoderWithDescriptor:passDesc];
                ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, encoder);
                [encoder endEncoding];
            }

            ImGuiContext* imguiContext = nullptr;
            graphics::metal::RenderDevice* deviceRef = nullptr;
            graphics::Graphics* graphicsRef = nullptr;
            InputBridge inputBridge;
            DrawCallback drawCallback;
            mutable std::mutex callbackMutex;
            Clock::time_point lastFrameTime;
            bool rendererInitialized = false;
            std::atomic<bool> enabled{true};
        };
    }

    std::unique_ptr<Backend> createBackendMetal(core::Engine& eng)
    {
        auto* device = eng.getGraphics().getDevice();
        if (!device || device->getDriver() != graphics::Driver::metal) return {};
        auto* metalDevice = static_cast<graphics::metal::RenderDevice*>(device);
        return std::make_unique<MetalBackend>(eng, *metalDevice);
    }
}

#endif // FLUXION_COMPILE_METAL
