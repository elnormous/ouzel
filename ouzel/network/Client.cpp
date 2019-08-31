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
            if (connected)
#ifdef _WIN32
                if (shutdown(sock, SD_BOTH) == SOCKET_ERROR)
                    throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to shutdown socket");
#else
                if (shutdown(sock, SHUT_RDWR) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to shutdown socket");
#endif
        }
    } // namespace network
} // namespace ouzel
