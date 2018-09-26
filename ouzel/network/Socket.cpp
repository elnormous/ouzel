// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifdef _WIN32
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <WinSock2.h>
#  include <WS2tcpip.h>
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
        Socket::Socket()
        {
        }

        Socket::~Socket()
        {
#ifdef _WIN32
            if (endpoint != INVALID_SOCKET) closesocket(endpoint);
#else
            if (endpoint != -1) close(endpoint);
#endif
        }

        Socket::Socket(Socket&& other)
        {
            endpoint = other.endpoint;

#ifdef _WIN32
            other.endpoint = INVALID_SOCKET;
#else
            other.endpoint = -1;
#endif
        }

        Socket& Socket::operator=(Socket&& other)
        {
            if (&other != this)
            {
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
            }

            return *this;
        }
    } // namespace network
} // namespace ouzel
