// Ouzel by Elviss Strazdins

#ifndef OUZEL_NETWORK_SOCKET_HPP
#define OUZEL_NETWORK_SOCKET_HPP

#include <stdexcept>
#include <system_error>

#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__unix__) || defined(__APPLE__)
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netdb.h>
#  include <unistd.h>
#  include <errno.h>
#endif

namespace ouzel::network
{
    inline namespace detail
    {
        inline int getLastError() noexcept
        {
#if defined(_WIN32)
            return WSAGetLastError();
#elif defined(__unix__) || defined(__APPLE__)
            return errno;
#endif
        }

#ifdef _WIN32
        constexpr auto closeSocket = closesocket;
#else
        constexpr auto closeSocket = ::close;
#endif
    }

    enum class InternetProtocol: std::uint8_t
    {
        v4,
        v6
    };

    constexpr int getAddressFamily(InternetProtocol internetProtocol)
    {
        return (internetProtocol == InternetProtocol::v4) ? AF_INET :
            (internetProtocol == InternetProtocol::v6) ? AF_INET6 :
            throw std::runtime_error("Unsupported protocol");
    }

    class Socket final
    {
    public:
#if defined(_WIN32)
        using Type = SOCKET;
        static constexpr Type invalid = INVALID_SOCKET;
#elif defined(__unix__) || defined(__APPLE__)
        using Type = int;
        static constexpr Type invalid = -1;
#endif

        explicit Socket(InternetProtocol internetProtocol = InternetProtocol::v4):
            endpoint{socket(getAddressFamily(internetProtocol), SOCK_STREAM, IPPROTO_TCP)}
        {
            if (endpoint == invalid)
                throw std::system_error(getLastError(), std::system_category(), "Failed to create socket");
        }

        explicit constexpr Socket(Type s) noexcept:
            endpoint(s)
        {
        }

        ~Socket()
        {
            if (endpoint != invalid) closeSocket(endpoint);
        }

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        Socket(Socket&& other) noexcept:
            endpoint{other.endpoint}
        {
            other.endpoint = invalid;
        }

        Socket& operator=(Socket&& other) noexcept
        {
            if (&other == this) return *this;

            if (endpoint != invalid) closeSocket(endpoint);
            endpoint = other.endpoint;
            other.endpoint = invalid;

            return *this;
        }

        operator Type() const noexcept { return endpoint; }

    private:
        Type endpoint = invalid;
    };
}

#endif // OUZEL_NETWORK_SOCKET_HPP
