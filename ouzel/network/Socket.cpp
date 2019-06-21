// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
#  include <WinSock2.h>
#  include <WS2tcpip.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <errno.h>
#  include <unistd.h>
#endif
#include "Socket.hpp"

namespace ouzel
{
    namespace network
    {
        Socket::Socket():
            endpoint(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))
        {
#ifdef _WIN32
            if (endpoint == INVALID_SOCKET)
                throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to create socket");
#else
            if (endpoint == -1)
                throw std::system_error(errno, std::system_category(), "Failed to create socket");
#endif
        }

        Socket::~Socket()
        {
#ifdef _WIN32
            if (endpoint != INVALID_SOCKET) closesocket(endpoint);
#else
            if (endpoint != -1) close(endpoint);
#endif
        }

        Socket::Socket(Socket&& other):
            endpoint(other.endpoint)
        {
#ifdef _WIN32
            other.endpoint = INVALID_SOCKET;
#else
            other.endpoint = -1;
#endif
        }

        Socket& Socket::operator=(Socket&& other)
        {
            if (&other == this) return *this;

#ifdef _WIN32
            if (endpoint != INVALID_SOCKET) closesocket(endpoint);
#else
            if (endpoint != -1) close(endpoint);
#endif

            endpoint = other.endpoint;

#ifdef _WIN32
            other.endpoint = INVALID_SOCKET;
#else
            other.endpoint = -1;
#endif

            return *this;
        }
    } // namespace network
} // namespace ouzel
