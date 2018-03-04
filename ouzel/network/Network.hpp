// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <errno.h>
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

            static bool getAddress(const std::string& address, uint32_t& result);

            bool init();

            bool listen(const std::string& address, uint16_t port);
            bool connect(const std::string& address, uint16_t port);
            bool disconnect();

        private:
#ifdef _WIN32
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif
        };
    } // namespace network
} // namespace ouzel
