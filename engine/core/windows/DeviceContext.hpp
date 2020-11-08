// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_DEVICECONTEXT_HPP
#define OUZEL_CORE_DEVICECONTEXT_HPP

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

namespace ouzel::core::windows
{
    class DeviceContext final
    {
    public:
        DeviceContext(HWND window = nullptr): dc{GetDC(window)}
        {
            if (!dc)
                throw std::runtime_error("Failed to get device context");
        }

        ~DeviceContext()
        {
            if (dc) ReleaseDC(nullptr, dc);
        }

        DeviceContext(DeviceContext&& other) noexcept:
            dc{other.dc}
        {
            other.dc = nullptr;
        }

        DeviceContext& operator=(DeviceContext&& other) noexcept
        {
            if (&other == this) return *this;
            if (dc) ReleaseDC(nullptr, dc);
            dc = other.dc;
            other.dc = nullptr;
            return *this;
        }

        operator HDC() const noexcept { return dc; }

    private:
        HDC dc = nullptr;
    };
}

#endif // OUZEL_CORE_DEVICECONTEXT_HPP
