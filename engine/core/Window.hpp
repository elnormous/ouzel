// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_WINDOW_HPP
#define OUZEL_CORE_WINDOW_HPP

#include <memory>
#include <string>
#include "NativeWindow.hpp"
#include "../graphics/Graphics.hpp"
#include "../math/Size.hpp"

namespace ouzel::core
{
    class Engine;

    class Window final
    {
    public:
        enum class Flags
        {
            none = 0x00,
            resizable = 0x01,
            fullscreen = 0x02,
            exclusiveFullscreen = 0x04,
            highDpi = 0x08
        };

        enum class Mode
        {
            windowed,
            windowedFullscreen,
            fullscreen
        };

        Window(Engine& initEngine,
               const math::Size<std::uint32_t, 2>& newSize,
               Flags flags,
               const std::string& newTitle,
               graphics::Driver graphicsDriver);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        auto getNativeWindow() const noexcept { return nativeWindow.get(); }

        void close();
        void update(bool waitForEvents);

        auto& getSize() const noexcept { return size; }
        void setSize(const math::Size<std::uint32_t, 2>& newSize);

        auto& getResolution() const noexcept { return resolution; }

        auto isResizable() const noexcept { return resizable; }

        void setFullscreen(bool newFullscreen);
        auto isFullscreen() const noexcept { return fullscreen; }

        auto isExclusiveFullscreen() const noexcept { return exclusiveFullscreen; }

        auto& getTitle() const noexcept { return title; }
        void setTitle(const std::string& newTitle);
        void bringToFront();
        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();

        auto isVisible() const noexcept { return visible; }
        auto isMinimized() const noexcept { return minimized; }

        auto convertWindowToNormalizedLocation(const math::Vector<float, 2>& position) const noexcept
        {
            return math::Vector<float, 2>{
                position.v[0] / static_cast<float>(size.v[0]),
                position.v[1] / static_cast<float>(size.v[1])
            };
        }

        auto convertNormalizedToWindowLocation(const math::Vector<float, 2>& position) const noexcept
        {
            return math::Vector<float, 2>{
                position.v[0] * static_cast<float>(size.v[0]),
                position.v[1] * static_cast<float>(size.v[1])
            };
        }

    private:
        void handleEvent(const NativeWindow::Event& event);

        Engine& engine;
        std::unique_ptr<NativeWindow> nativeWindow;

        math::Size<std::uint32_t, 2> size{};
        math::Size<std::uint32_t, 2> resolution{};
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;
        bool visible = false;
        bool minimized = false;
        std::uint32_t displayId = 0;

        std::string title;
    };

    inline constexpr Window::Flags operator&(const Window::Flags a, const Window::Flags b) noexcept
    {
        return static_cast<Window::Flags>(static_cast<std::underlying_type_t<Window::Flags>>(a) & static_cast<std::underlying_type_t<Window::Flags>>(b));
    }
    inline constexpr Window::Flags operator|(const Window::Flags a, const Window::Flags b) noexcept
    {
        return static_cast<Window::Flags>(static_cast<std::underlying_type_t<Window::Flags>>(a) | static_cast<std::underlying_type_t<Window::Flags>>(b));
    }
    inline constexpr Window::Flags operator^(const Window::Flags a, const Window::Flags b) noexcept
    {
        return static_cast<Window::Flags>(static_cast<std::underlying_type_t<Window::Flags>>(a) ^ static_cast<std::underlying_type_t<Window::Flags>>(b));
    }
    inline constexpr Window::Flags operator~(const Window::Flags a) noexcept
    {
        return static_cast<Window::Flags>(~static_cast<std::underlying_type_t<Window::Flags>>(a));
    }
    inline constexpr Window::Flags& operator&=(Window::Flags& a, const Window::Flags b) noexcept
    {
        return a = static_cast<Window::Flags>(static_cast<std::underlying_type_t<Window::Flags>>(a) & static_cast<std::underlying_type_t<Window::Flags>>(b));
    }
    inline constexpr Window::Flags& operator|=(Window::Flags& a, const Window::Flags b) noexcept
    {
        return a = static_cast<Window::Flags>(static_cast<std::underlying_type_t<Window::Flags>>(a) | static_cast<std::underlying_type_t<Window::Flags>>(b));
    }
    inline constexpr Window::Flags& operator^=(Window::Flags& a, const Window::Flags b) noexcept
    {
        return a = static_cast<Window::Flags>(static_cast<std::underlying_type_t<Window::Flags>>(a) ^ static_cast<std::underlying_type_t<Window::Flags>>(b));
    }
}

#endif // OUZEL_CORE_WINDOW_HPP
