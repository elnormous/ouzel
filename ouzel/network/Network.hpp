// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_NETWORK_NETWORK_HPP
#define OUZEL_NETWORK_NETWORK_HPP

#ifdef _WIN32
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
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

        constexpr uint32_t ANY_ADDRESS = 0;
        constexpr uint16_t ANY_PORT = 0;

        class Network final
        {
        public:
            Network();
            ~Network();

            Network(const Network&) = delete;
            Network& operator=(const Network&) = delete;

            Network(Network&&) = delete;
            Network& operator=(Network&&) = delete;

            static uint32_t getAddress(const std::string& address);

        private:
#ifdef _WIN32
            bool wsaStarted = false;
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif

            std::vector<std::unique_ptr<Client>> clients;
        };
    } // namespace network
} // namespace ouzel

#endif // OUZEL_NETWORK_NETWORK_HPP
