// Copyright (C) 2018 Elviss Strazdins
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
        const uint32_t ANY_ADDRESS = 0;
        const uint16_t ANY_PORT = 0;

        class Network final
        {
        public:
            Network();
            ~Network();

            static int getLastError();
            static bool getAddress(const std::string& address, uint32_t& result);

            bool init();

            bool listen(const std::string& address, uint16_t port);
            bool connect(const std::string& address, uint16_t port);
            bool disconnect();

        private:
            Socket endpoint = NULL_SOCKET;
        };
    } // namespace network
} // namespace ouzel
