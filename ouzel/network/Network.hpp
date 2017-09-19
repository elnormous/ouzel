// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
typedef SOCKET Socket;
#else
#include <errno.h>
typedef int Socket;
#define INVALID_SOCKET -1
#endif

#include <cstdint>
#include <string>

namespace ouzel
{
    namespace network
    {
        inline int getLastError()
        {
#ifdef _WIN32
            return WSAGetLastError();
#else
            return errno;
#endif
        }

        class Network
        {
        public:
            Network();
            ~Network();

            bool init();

            bool startServer(const std::string& address, uint16_t port);
            bool connect(const std::string& address, uint16_t port);
            bool disconnect();

        private:
            Socket endpoint = INVALID_SOCKET;
        };
    } // namespace network
} // namespace ouzel
