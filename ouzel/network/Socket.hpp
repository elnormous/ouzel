// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_NETWORK_SOCKET_HPP
#define OUZEL_NETWORK_SOCKET_HPP

#include <stdexcept>
#include <system_error>

#ifdef _WIN32
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
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netdb.h>
#  include <unistd.h>
#  include <errno.h>
#endif

namespace ouzel
{
    namespace network
    {
        inline int getLastError() noexcept
        {
#ifdef _WIN32
            return WSAGetLastError();
#else
            return errno;
#endif
        }

        enum class InternetProtocol: uint8_t
        {
            V4,
            V6
        };

        constexpr int getAddressFamily(InternetProtocol internetProtocol)
        {
            return (internetProtocol == InternetProtocol::V4) ? AF_INET :
                (internetProtocol == InternetProtocol::V6) ? AF_INET6 :
                throw std::runtime_error("Unsupported protocol");
        }

        class Socket final
        {
        public:
#ifdef _WIN32
            using Type = SOCKET;
            static constexpr Type Invalid = INVALID_SOCKET;
#else
            using Type = int;
            static constexpr Type Invalid = -1;
#endif

            explicit Socket(InternetProtocol internetProtocol = InternetProtocol::V4):
                endpoint(socket(getAddressFamily(internetProtocol), SOCK_STREAM, IPPROTO_TCP))
            {
                if (endpoint == Invalid)
                    throw std::system_error(getLastError(), std::system_category(), "Failed to create socket");
            }

            explicit constexpr Socket(Type s) noexcept:
                endpoint(s)
            {
            }

            ~Socket()
            {
                if (endpoint != Invalid)
#ifdef _WIN32
                    closesocket(endpoint);
#else
                    close(endpoint);
#endif
            }

            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;

            Socket(Socket&& other) noexcept:
                endpoint(other.endpoint)
            {
                other.endpoint = Invalid;
            }

            Socket& operator=(Socket&& other) noexcept
            {
                if (&other != this)
                {
                    if (endpoint != Invalid)
#ifdef _WIN32
                        closesocket(endpoint);
#else
                        close(endpoint);
#endif
                    endpoint = other.endpoint;
                    other.endpoint = Invalid;
                }

                return *this;
            }

            inline operator Type() const noexcept { return endpoint; }

        private:
            Type endpoint = Invalid;
        };
    } // namespace network
} // namespace ouzel

#endif // OUZEL_NETWORK_SOCKET_HPP
