// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#ifdef _WIN32
#include <winsock.h>
#endif

namespace ouzel
{
    namespace network
    {
        class Socket
        {
        public:
            Socket();
            ~Socket();

            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;

            Socket(Socket&& other);
            Socket& operator=(Socket&& other);

        private:
#ifdef _WIN32
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif
        };
    } // namespace network
} // namespace ouzel
