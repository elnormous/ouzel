// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_NATIVEWINDOWWIN_HPP
#define OUZEL_CORE_NATIVEWINDOWWIN_HPP

#include <set>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "../NativeWindow.hpp"

namespace ouzel::core::windows
{
    class Engine;

    class NativeWindow final: public core::NativeWindow
    {
        friend Engine;
    public:
        NativeWindow(const std::function<void(const Event&)>& initCallback,
                     const math::Size<std::uint32_t, 2>& newSize,
                     bool newResizable,
                     bool newFullscreen,
                     bool newExclusiveFullscreen,
                     const std::string& newTitle,
                     bool newHighDpi);
        ~NativeWindow() override;

        void close();

        void setSize(const math::Size<std::uint32_t, 2>& newSize);
        void setFullscreen(bool newFullscreen);
        void setTitle(const std::string& newTitle);
        void bringToFront();
        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();

        void handleResize(const math::Size<std::uint32_t, 2>& newSize);
        void handleMove();
        void handleActivate(WPARAM wParam);
        void handleShowWindow(BOOL shown);
        void handleMinimize();
        void handleMaximize();
        void handleRestore();
        void handleKey(UINT message, WPARAM wParam, LPARAM lParam);
        void handleMouseMove(LPARAM lParam);
        void handleMouseButton(UINT message, WPARAM wParam, LPARAM lParam);
        void handleMouseWheel(UINT message, WPARAM wParam, LPARAM lParam);
        void handleTouch(WPARAM wParam, LPARAM lParam);

        auto getNativeWindow() const noexcept { return window; }
        void addAccelerator(HACCEL accelerator);
        void removeAccelerator(HACCEL accelerator);

        auto getMonitor() const noexcept { return monitor; }

    private:
        void executeCommand(const Command& command) final;
        void switchFullscreen(bool newFullscreen);

        ATOM windowClass = 0;
        HWND window = nullptr;
        DWORD windowStyle = 0;
        DWORD windowExStyle = 0;
        DWORD windowWindowedStyle = 0;
        DWORD windowFullscreenStyle = 0;
        HMONITOR monitor = nullptr;

        int windowX = 0;
        int windowY = 0;
        int windowWidth = 0;
        int windowHeight = 0;

        std::set<HACCEL> accelerators;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWWIN_HPP
