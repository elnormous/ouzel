#include "../../core/Setup.h"

#if FLUXION_COMPILE_OPENGL

#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include "ImGuiBackend.hpp"
#include "ImGuiInputBridge.hpp"
#include "../../core/Engine.hpp"
#include "../../graphics/Graphics.hpp"
#include "../../graphics/opengl/OGLRenderDevice.hpp"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

namespace fluxion::gui::imgui
{
    namespace
    {
        class OGLBackend final : public Backend
        {
        public:
            OGLBackend(core::Engine& eng,
                       graphics::opengl::RenderDevice& oglDevice):
                imguiContext{ImGui::CreateContext()},
                deviceRef{&oglDevice},
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
                io.BackendRendererName = "imgui_impl_opengl3";

                ImGui::StyleColorsDark();

                deviceRef->setPreSwapHook([this]() { renderFrame(); });
            }

            ~OGLBackend() override
            {
                if (deviceRef) deviceRef->setPreSwapHook({});

                // Init/Shutdown for the OpenGL3 backend MUST run on the render
                // thread because the GL context is current there. We schedule
                // a synchronous shutdown if Init has happened, so device
                // objects (VBO/VAO/program/font texture) are released cleanly.
                if (deviceRef && rendererInitialized.load(std::memory_order_acquire))
                {
                    std::promise<void> done;
                    auto future = done.get_future();
                    deviceRef->executeOnRenderThread([this, &done]() {
                        ImGui::SetCurrentContext(imguiContext);
                        ImGui_ImplOpenGL3_Shutdown();
                        done.set_value();
                    });
                    future.wait();
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

                // Lazy renderer init on the render thread (where the GL context
                // is current). Subsequent frames skip this branch.
                if (!rendererInitialized.load(std::memory_order_acquire))
                {
                    if (!ImGui_ImplOpenGL3_Init(nullptr))
                        throw std::runtime_error{"ImGui_ImplOpenGL3_Init failed"};
                    rendererInitialized.store(true, std::memory_order_release);
                }

                const auto now = Clock::now();
                const float dt = std::chrono::duration<float>{now - lastFrameTime}.count();
                lastFrameTime = now;

                inputBridge.newFrame(graphicsRef->getSize(), dt);

                ImGui_ImplOpenGL3_NewFrame();
                ImGui::NewFrame();
                inputBridge.snapshotWantCapture(ImGui::GetIO());

                DrawCallback cb;
                {
                    std::lock_guard lock{callbackMutex};
                    cb = drawCallback;
                }
                if (cb) cb();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            ImGuiContext* imguiContext = nullptr;
            graphics::opengl::RenderDevice* deviceRef = nullptr;
            graphics::Graphics* graphicsRef = nullptr;
            InputBridge inputBridge;
            DrawCallback drawCallback;
            mutable std::mutex callbackMutex;
            Clock::time_point lastFrameTime;
            std::atomic<bool> rendererInitialized{false};
            std::atomic<bool> enabled{true};
        };
    }

    std::unique_ptr<Backend> createBackendOGL(core::Engine& eng)
    {
        auto* device = eng.getGraphics().getDevice();
        if (!device || device->getDriver() != graphics::Driver::openGl) return {};
        auto* oglDevice = static_cast<graphics::opengl::RenderDevice*>(device);
        return std::make_unique<OGLBackend>(eng, *oglDevice);
    }
}

#endif // FLUXION_COMPILE_OPENGL
