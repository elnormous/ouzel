#include "../../core/Setup.h"

#if FLUXION_COMPILE_DIRECT3D11

#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include "ImGuiBackend.hpp"
#include "ImGuiInputBridge.hpp"
#include "../../core/Engine.hpp"
#include "../../graphics/Graphics.hpp"
#include "../../graphics/Texture.hpp"
#include "../../graphics/direct3d11/D3D11RenderDevice.hpp"
#include "../../graphics/direct3d11/D3D11Texture.hpp"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace fluxion::gui::imgui
{
    namespace
    {
        class D3D11Backend final : public Backend
        {
        public:
            D3D11Backend(core::Engine& eng,
                         graphics::d3d11::RenderDevice& d3d11Device):
                imguiContext{ImGui::CreateContext()},
                deviceRef{&d3d11Device},
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
                io.BackendRendererName = "imgui_impl_dx11";

                ImGui::StyleColorsDark();

                // ImGui_ImplDX11_Init() merely caches the device/context pointers;
                // GPU resource creation is deferred to the first NewFrame call,
                // so it is safe to call here from the main thread.
                if (!ImGui_ImplDX11_Init(d3d11Device.getDevice().get(),
                                         d3d11Device.getContext().get()))
                    throw std::runtime_error{"ImGui_ImplDX11_Init failed"};

                d3d11Device.setPreSwapHook([this]() { renderFrame(); });
            }

            ~D3D11Backend() override
            {
                // Disconnect the hook before tearing anything down so the render
                // thread does not call into a half-destroyed backend.
                if (deviceRef) deviceRef->setPreSwapHook({});

                // ImGui_ImplDX11_Shutdown releases D3D11 resources via the
                // device context. The render thread must do that, otherwise
                // we either crash on free or leak. We schedule it and wait.
                if (deviceRef && rendererInitialized)
                {
                    std::promise<void> done;
                    auto future = done.get_future();
                    deviceRef->executeOnRenderThread([this, &done]() {
                        ImGui::SetCurrentContext(imguiContext);
                        ImGui_ImplDX11_Shutdown();
                        done.set_value();
                    });
                    future.wait();
                }
                else
                {
                    // Hook never fired (engine never rendered): no GPU state
                    // was ever created, so calling Shutdown from this thread
                    // is safe.
                    ImGui::SetCurrentContext(imguiContext);
                    ImGui_ImplDX11_Shutdown();
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

            std::uint64_t textureNativeHandle(
                const graphics::Texture& texture) const noexcept override
            {
                if (!deviceRef) return 0;
                const std::size_t id = static_cast<std::size_t>(texture.getResource());
                if (!id) return 0;
                auto* d3dTex = deviceRef->getResource<graphics::d3d11::Texture>(id);
                if (!d3dTex) return 0;
                auto* srv = d3dTex->getResourceView().get();
                // ImGui's D3D11 backend treats ImTextureID as the SRV
                // pointer cast to a 64-bit integer.
                return reinterpret_cast<std::uint64_t>(srv);
            }

        private:
            using Clock = std::chrono::steady_clock;

            void renderFrame()
            {
                if (!enabled.load(std::memory_order_acquire))
                {
                    // Master switch off: skip the entire frame. The input
                    // bridge keeps buffering events behind the scenes so
                    // toggling back on resumes seamlessly with up-to-date
                    // mouse position on the first re-enabled frame.
                    return;
                }

                ImGui::SetCurrentContext(imguiContext);
                rendererInitialized = true;

                const auto now = Clock::now();
                const float dt = std::chrono::duration<float>{now - lastFrameTime}.count();
                lastFrameTime = now;

                inputBridge.newFrame(graphicsRef->getSize(), dt);

                ImGui_ImplDX11_NewFrame();
                ImGui::NewFrame();
                inputBridge.snapshotWantCapture(ImGui::GetIO());

                DrawCallback cb;
                {
                    std::lock_guard lock{callbackMutex};
                    cb = drawCallback;
                }
                if (cb) cb();

                ImGui::Render();
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            }

            ImGuiContext* imguiContext = nullptr;
            graphics::d3d11::RenderDevice* deviceRef = nullptr;
            graphics::Graphics* graphicsRef = nullptr;
            InputBridge inputBridge;
            DrawCallback drawCallback;
            mutable std::mutex callbackMutex;
            Clock::time_point lastFrameTime;
            bool rendererInitialized = false;
            std::atomic<bool> enabled{true};
        };
    }

    std::unique_ptr<Backend> createBackendD3D11(core::Engine& eng)
    {
        auto* device = eng.getGraphics().getDevice();
        if (!device || device->getDriver() != graphics::Driver::direct3D11) return {};
        auto* d3d11Device = static_cast<graphics::d3d11::RenderDevice*>(device);
        return std::make_unique<D3D11Backend>(eng, *d3d11Device);
    }
}

#endif // FLUXION_COMPILE_DIRECT3D11
