// Ouzel by Elviss Strazdins

#include "Server.hpp"
#include "Network.hpp"

namespace ouzel::network
{
    Server::Server(Network& initNetwork):
        network(&initNetwork)
    {
    }

    Server::~Server()
    {
    }

    void Server::listen(const std::string&, std::uint16_t)
    {
    }

    void Server::disconnect()
    {
        if (connected)
#if defined(_WIN32)
            if (shutdown(sock, SD_BOTH) == SOCKET_ERROR)
                throw std::system_error(WSAGetLastError(), std::system_category(), "Failed to shutdown socket");
#elif defined(__unix__) || defined(__APPLE__)
            if (shutdown(sock, SHUT_RDWR) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to shutdown socket");
#endif
    }
}
