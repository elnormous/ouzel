// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_NETWORK_SERVER_HPP
#define OUZEL_NETWORK_SERVER_HPP

#include "Socket.hpp"
#include "../thread/Thread.hpp"

namespace ouzel::network
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
        thread::Thread readThread;
        thread::Thread writeThread;
        bool connected = false;
    };
}

#endif // OUZEL_NETWORK_SERVER_HPP
