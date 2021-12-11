// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_WINAPI_WINSOCK_HPP
#define OUZEL_PLATFORM_WINAPI_WINSOCK_HPP

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <winsock2.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

namespace ouzel::platform::winapi
{
    class WinSock final
    {
    public:
        WinSock()
        {
            WSADATA wsaData;
            if (const auto error = WSAStartup(MAKEWORD(2, 2), &wsaData); error != 0)
                throw std::system_error{error, std::system_category(), "WSAStartup failed"};

            if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
            {
                WSACleanup();
                throw std::runtime_error{"Invalid WinSock version"};
            }

            started = true;
        }

        ~WinSock()
        {
            if (started) WSACleanup();
        }

        WinSock(WinSock&& other) noexcept :
            started(other.started)
        {
            other.started = false;
        }

        WinSock& operator=(WinSock&& other) noexcept
        {
            if (&other == this) return *this;
            if (started) WSACleanup();
            started = other.started;
            other.started = false;
            return *this;
        }

    private:
        bool started = false;
    };
}

#endif // OUZEL_PLATFORM_WINAPI_WINSOCK_HPP
