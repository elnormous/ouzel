// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_WINDOW_HPP
#define OUZEL_CORE_WINDOW_HPP

#include <cstdint>
#include <memory>
#include <queue>
#include <string>
#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif
#if TARGET_OS_IOS
#  include "ios/NativeWindowIOS.hpp"
#elif TARGET_OS_TV
#  include "tvos/NativeWindowTVOS.hpp"
#elif TARGET_OS_MAC
#  include "macos/NativeWindowMacOS.hpp"
#elif defined(__ANDROID__)
#  include "android/NativeWindowAndroid.hpp"
#elif defined(__linux__)
#  include "linux/NativeWindowLinux.hpp"
#elif defined(_WIN32)
#  include "windows/NativeWindowWin.hpp"
#elif defined(__EMSCRIPTEN__)
#  include "emscripten/NativeWindowEm.hpp"
#endif
#include "../events/Event.hpp"
#include "../graphics/Driver.hpp"
#include "../math/Size.hpp"

namespace ouzel::core
{
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

        Window(const math::Size<std::uint32_t, 2>& newSize,
               Flags flags,
               const std::string& newTitle,
               graphics::Driver graphicsDriver);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        auto& getNativeWindow() noexcept { return nativeWindow; }
        auto& getNativeWindow() const noexcept { return nativeWindow; }

        void close();
        std::queue<std::unique_ptr<Event>> getEvents(bool waitForEvents);

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
#if TARGET_OS_IOS
        ios::NativeWindow nativeWindow;
#elif TARGET_OS_TV
        tvos::NativeWindow nativeWindow;
#elif TARGET_OS_MAC
        macos::NativeWindow nativeWindow;
#elif defined(__ANDROID__)
        android::NativeWindow nativeWindow;
#elif defined(__linux__)
        linux::NativeWindow nativeWindow;
#elif defined(_WIN32)
        windows::NativeWindow nativeWindow;
#elif defined(__EMSCRIPTEN__)
        emscripten::NativeWindow nativeWindow;
#else
        NativeWindow nativeWindow;
#endif

        math::Size<std::uint32_t, 2> size{};
        math::Size<std::uint32_t, 2> resolution{};
        bool resizable = false;
        bool fullscreen = false;
        bool exclusiveFullscreen = false;
        bool highDpi = true;
        bool visible = false;
        bool minimized = false;
        std::uintptr_t displayId = 0;

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
