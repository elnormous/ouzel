#ifndef FLUXION_GUI_IMGUI_IMGUIINPUTBRIDGE_HPP
#define FLUXION_GUI_IMGUI_IMGUIINPUTBRIDGE_HPP

#include <atomic>
#include <mutex>
#include <vector>
#include "imgui.h"
#include "../../events/EventHandler.hpp"
#include "../../math/Size.hpp"

namespace fluxion
{
    class EventDispatcher;
}

namespace fluxion::gui::imgui
{
    // Bridges Fluxion EventDispatcher input events into ImGuiIO. Designed for
    // multi-threaded engines: input events fire on the platform/window thread,
    // ImGui state lives on the render thread. The bridge buffers events under
    // a mutex and drains them into ImGuiIO at the start of each ImGui frame.
    //
    // It also filters input propagation: when ImGui captures the mouse or
    // keyboard, the EventHandler returns true so that the rest of the engine's
    // handlers (game scenes etc.) never see the consumed event. The capture
    // flags are snapshotted via std::atomic so that the dispatch thread can
    // read them without locking ImGui state.
    class InputBridge final
    {
    public:
        explicit InputBridge(EventDispatcher& dispatcher);
        ~InputBridge();

        InputBridge(const InputBridge&) = delete;
        InputBridge& operator=(const InputBridge&) = delete;
        InputBridge(InputBridge&&) = delete;
        InputBridge& operator=(InputBridge&&) = delete;

        // Render-thread entry point. Drains buffered input into ImGuiIO and
        // updates DisplaySize / DeltaTime. Caller must have set the current
        // ImGui context first.
        void newFrame(const math::Size<std::uint32_t, 2>& displaySize,
                      float deltaTime);

        // Called by the backend after ImGui::NewFrame has populated WantCapture
        // flags, so the next batch of dispatched events sees up-to-date state.
        void snapshotWantCapture(const ImGuiIO& io) noexcept;

        bool wantsKeyboard() const noexcept { return wantsKeyboard_.load(std::memory_order_acquire); }
        bool wantsMouse() const noexcept { return wantsMouse_.load(std::memory_order_acquire); }

    private:
        struct BufferedEvent
        {
            enum class Kind : std::uint8_t { mouseMove, mouseButton, mouseScroll, key, focus };
            Kind kind;
            // mouseMove: normalized [0..1] window position, Fluxion Y-up
            float nx = 0.0F, ny = 0.0F;
            // mouseButton: ImGui index (0..4); down flag in `pressed`
            // mouseScroll: dx, dy in `nx`, `ny`
            int button = -1;
            ImGuiKey key = ImGuiKey_None;
            bool pressed = false;
        };

        bool handleMouse(const MouseEvent& event);
        bool handleKeyboard(const KeyboardEvent& event);

        EventDispatcher& dispatcher_;
        EventHandler handler_;

        std::mutex bufferMutex_;
        std::vector<BufferedEvent> buffer_;

        std::atomic<bool> wantsKeyboard_{false};
        std::atomic<bool> wantsMouse_{false};
    };
}

#endif // FLUXION_GUI_IMGUI_IMGUIINPUTBRIDGE_HPP
