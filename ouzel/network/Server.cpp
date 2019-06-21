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

        Server::Server(Server&& other):
            sock(std::move(other.sock))
        {
        }

        Server& Server::operator=(Server&& other)
        {
            if (&other == this) return *this;

            sock = std::move(other.sock);

            return *this;
        }

        void Server::listen(const std::string& address, uint16_t port)
        {
        }

        void Server::disconnect()
        {
        }
    } // namespace network
} // namespace ouzel
