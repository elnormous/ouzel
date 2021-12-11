// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_WINAPI_DEVICECONTEXT_HPP
#define OUZEL_PLATFORM_WINAPI_DEVICECONTEXT_HPP

#include <stdexcept>
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

namespace ouzel::platform::winapi
{
    class DeviceContext final
    {
    public:
        DeviceContext(HWND win = nullptr):
            window{win},
            dc{GetDC(window)}
        {
            if (!dc)
                throw std::runtime_error{"Failed to get device context"};
        }

        ~DeviceContext()
        {
            if (dc) ReleaseDC(window, dc);
        }

        DeviceContext(DeviceContext&& other) noexcept:
            window{other.window},
            dc{other.dc}
        {
            other.window = nullptr;
            other.dc = nullptr;
        }

        DeviceContext& operator=(DeviceContext&& other) noexcept
        {
            if (&other == this) return *this;
            if (dc) ReleaseDC(window, dc);
            window = other.window;
            dc = other.dc;
            other.window = nullptr;
            other.dc = nullptr;
            return *this;
        }

        auto get() const noexcept { return dc; }

    private:
        HWND window = nullptr;
        HDC dc = nullptr;
    };
}

#endif // OUZEL_PLATFORM_WINAPI_DEVICECONTEXT_HPP
