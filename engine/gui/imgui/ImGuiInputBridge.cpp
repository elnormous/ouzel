#include "ImGuiInputBridge.hpp"
#include "ImGuiKeyMap.hpp"
#include "../../events/EventDispatcher.hpp"
#include "../../events/Event.hpp"

namespace fluxion::gui::imgui
{
    InputBridge::InputBridge(EventDispatcher& dispatcher):
        dispatcher_{dispatcher},
        handler_{EventHandler::priorityMax}
    {
        handler_.mouseHandler = [this](const MouseEvent& e) { return handleMouse(e); };
        handler_.keyboardHandler = [this](const KeyboardEvent& e) { return handleKeyboard(e); };
        dispatcher_.addEventHandler(handler_);
    }

    InputBridge::~InputBridge()
    {
        // EventHandler destructor also unregisters, but explicit removal
        // ensures it happens before our `this`-capturing handlers go away.
        handler_.remove();
    }

    bool InputBridge::handleMouse(const MouseEvent& event)
    {
        BufferedEvent be{};
        switch (event.type)
        {
            case Event::Type::mousePress:
            case Event::Type::mouseRelease:
            {
                const int btn = toImGuiMouseButton(event.button);
                if (btn < 0) return false;
                be.kind = BufferedEvent::Kind::mouseButton;
                be.button = btn;
                be.pressed = (event.type == Event::Type::mousePress);
                break;
            }
            case Event::Type::mouseMove:
                be.kind = BufferedEvent::Kind::mouseMove;
                be.nx = event.position.v[0];
                be.ny = event.position.v[1];
                break;
            case Event::Type::mouseScroll:
                be.kind = BufferedEvent::Kind::mouseScroll;
                be.nx = event.scroll.v[0];
                be.ny = event.scroll.v[1];
                break;
            default:
                return false;
        }

        {
            std::lock_guard lock{bufferMutex_};
            buffer_.push_back(be);
        }

        return wantsMouse_.load(std::memory_order_acquire);
    }

    bool InputBridge::handleKeyboard(const KeyboardEvent& event)
    {
        if (event.type != Event::Type::keyboardKeyPress &&
            event.type != Event::Type::keyboardKeyRelease)
            return false;

        const ImGuiKey k = toImGuiKey(event.key);
        if (k == ImGuiKey_None) return false;

        BufferedEvent be{};
        be.kind = BufferedEvent::Kind::key;
        be.key = k;
        be.pressed = (event.type == Event::Type::keyboardKeyPress);

        {
            std::lock_guard lock{bufferMutex_};
            buffer_.push_back(be);
        }

        return wantsKeyboard_.load(std::memory_order_acquire);
    }

    void InputBridge::newFrame(const math::Size<std::uint32_t, 2>& displaySize,
                               float deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2{static_cast<float>(displaySize.v[0]),
                                static_cast<float>(displaySize.v[1])};
        io.DeltaTime = deltaTime > 0.0F ? deltaTime : 1.0F / 60.0F;

        std::vector<BufferedEvent> frameBuffer;
        {
            std::lock_guard lock{bufferMutex_};
            frameBuffer.swap(buffer_);
        }

        const float w = io.DisplaySize.x;
        const float h = io.DisplaySize.y;

        for (const auto& e : frameBuffer)
        {
            switch (e.kind)
            {
                case BufferedEvent::Kind::mouseMove:
                    // Window::convertWindowToNormalizedLocation hands us window-relative
                    // coords already in Y-down normalized space (0,0 = top-left,
                    // 1,1 = bottom-right). ImGui also uses Y-down pixel coords, so
                    // we just scale by display size — no flip needed.
                    io.AddMousePosEvent(e.nx * w, e.ny * h);
                    break;
                case BufferedEvent::Kind::mouseButton:
                    io.AddMouseButtonEvent(e.button, e.pressed);
                    break;
                case BufferedEvent::Kind::mouseScroll:
                    // Fluxion's wheel convention: scroll-up yields negative Y
                    // (Win32 wParam HIWORD is sign-flipped at the platform layer).
                    // ImGui's convention: scroll-up yields positive Y. Flip.
                    io.AddMouseWheelEvent(e.nx, -e.ny);
                    break;
                case BufferedEvent::Kind::key:
                    io.AddKeyEvent(e.key, e.pressed);
                    break;
                case BufferedEvent::Kind::focus:
                    io.AddFocusEvent(e.pressed);
                    break;
            }
        }
    }

    void InputBridge::snapshotWantCapture(const ImGuiIO& io) noexcept
    {
        wantsKeyboard_.store(io.WantCaptureKeyboard, std::memory_order_release);
        wantsMouse_.store(io.WantCaptureMouse, std::memory_order_release);
    }
}
