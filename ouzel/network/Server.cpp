// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Server.hpp"
#include "Network.hpp"

namespace ouzel
{
    namespace network
    {
        Server::Server(Network& initNetwork):
            network(&initNetwork)
        {
        }

        Server::~Server()
        {
        }

        void Server::listen(const std::string& address, uint16_t port)
        {
        }

        void Server::disconnect()
        {
        }
    } // namespace network
} // namespace ouzel
