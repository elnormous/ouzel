// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <utility>
#include "Client.hpp"

namespace ouzel
{
    namespace network
    {
        Client::~Client()
        {
        }

        Client::Client(Client&& other)
        {
            sock = std::move(other.sock);
        }

        Client& Client::operator=(Client&& other)
        {
            if (&other != this)
            {
                sock = std::move(other.sock);
            }

            return *this;
        }

        void Client::disconnect()
        {
        }
    } // namespace network
} // namespace ouzel
