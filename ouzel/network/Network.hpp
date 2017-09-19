// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#ifdef _WIN32
#include <basetsd.h>
typedef UINT_PTR Socket;
#define NULL_SOCKET static_cast<Socket>(~0)
#else
#include <errno.h>
typedef int Socket;
#define NULL_SOCKET -1
#endif

#include <cstdint>
#include <string>

namespace ouzel
{
    namespace network
    {
        class Network
        {
        public:
            Network();
            ~Network();

            static int getLastError();

            bool init();

            bool startServer(const std::string& address, uint16_t port);
            bool connect(const std::string& address, uint16_t port);
            bool disconnect();

        private:
            Socket endpoint = NULL_SOCKET;
        };
    } // namespace network
} // namespace ouzel
