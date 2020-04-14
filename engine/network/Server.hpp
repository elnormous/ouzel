// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_NETWORK_SERVER_HPP
#define OUZEL_NETWORK_SERVER_HPP

#include "Socket.hpp"
#include "utils/Thread.hpp"

namespace ouzel
{
    namespace network
    {
        class Network;

        class Server final
        {
        public:
            explicit Server(Network& initNetwork);
            ~Server();

            Server(const Server&) = delete;
            Server& operator=(const Server&) = delete;

            void listen(const std::string& address, std::uint16_t port);
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

#endif // OUZEL_NETWORK_SERVER_HPP
