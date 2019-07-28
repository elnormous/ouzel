// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Client.hpp"
#include "Network.hpp"

namespace ouzel
{
    namespace network
    {
        Client::Client(Network& initNetwork):
            network(&initNetwork)
        {
        }

        Client::~Client()
        {
        }

        void Client::connect(const std::string& address, uint16_t port)
        {
        }

        void Client::disconnect()
        {
        }
    } // namespace network
} // namespace ouzel
