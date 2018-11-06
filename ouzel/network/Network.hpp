// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#ifdef _WIN32
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock.h>
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ouzel
{
    namespace network
    {
        class Client;

        const uint32_t ANY_ADDRESS = 0;
        const uint16_t ANY_PORT = 0;

        class Network final
        {
        public:
            Network();
            ~Network();

            Network(const Network&) = delete;
            Network& operator=(const Network&) = delete;

            Network(Network&&) = delete;
            Network& operator=(Network&&) = delete;

            static void getAddress(const std::string& address, uint32_t& result);

            void listen(const std::string& address, uint16_t port);
            void connect(const std::string& address, uint16_t port);
            void disconnect();

        private:
#ifdef _WIN32
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif

            std::vector<std::unique_ptr<Client>> clients;
        };
    } // namespace network
} // namespace ouzel
