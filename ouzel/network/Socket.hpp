// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_NETWORK_SOCKET_HPP
#define OUZEL_NETWORK_SOCKET_HPP

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <winsock.h>
#  undef WIN32_LEAN_AND_MEAN
#  undef NOMINMAX
#endif

namespace ouzel
{
    namespace network
    {
        class Socket final
        {
        public:
            Socket();
#ifdef _WIN32
            Socket(SOCKET s);
#else
            Socket(int s);
#endif
            ~Socket();

            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;

            Socket(Socket&& other);
            Socket& operator=(Socket&& other);

#ifdef _WIN32
            operator SOCKET() const { return endpoint; }
#else
            operator int() const { return endpoint; }
#endif

        private:
#ifdef _WIN32
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif
        };
    } // namespace network
} // namespace ouzel

#endif // OUZEL_NETWORK_SOCKET_HPP
