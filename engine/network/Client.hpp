// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_NETWORK_CLIENT_HPP
#define OUZEL_NETWORK_CLIENT_HPP

#include "network/Socket.hpp"
#include "utils/Thread.hpp"

namespace ouzel
{
    namespace network
    {
        class Network;

        class Client final
        {
        public:
            explicit Client(Network& initNetwork);
            ~Client();

            Client(const Client&) = delete;
            Client& operator=(const Client&) = delete;

            void connect(const std::string& address, std::uint16_t port);
            void disconnect();

        private:
            Network* network = nullptr;
            Socket sock;
            Thread readThread;
            Thread writeThread;
            bool connected = false;
        };
    } // namespace network
} // namespace ouzel

#endif // OUZEL_NETWORK_CLIENT_HPP
