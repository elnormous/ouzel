// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <system_error>
#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <WinSock2.h>
#  include <WS2tcpip.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__unix__) || defined(__APPLE__)
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <errno.h>
#  include <poll.h>
#  include <netdb.h>
#  include <unistd.h>
#endif
#include "Network.hpp"
#include "Client.hpp"

namespace ouzel::network
{
    Network::Network()
    {
#if defined(_WIN32)
        const auto sockVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        const auto error = WSAStartup(sockVersion, &wsaData);
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to start WinSock failed");

        if (wsaData.wVersion != sockVersion)
        {
            WSACleanup();
            throw std::runtime_error("Invalid WinSock version");
        }

        wsaVersion = wsaData.wVersion;
#endif
    }

    Network::~Network()
    {
#if defined(_WIN32)
        if (wsaVersion) WSACleanup();
#endif
    }

    std::uint32_t Network::getAddress(const std::string& address)
    {
        addrinfo* info;
        const auto ret = getaddrinfo(address.c_str(), nullptr, nullptr, &info);

        if (ret != 0)
            throw std::system_error(errno, std::system_category(), "Failed to get address info of " + address);

        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
        const std::uint32_t result = ntohl(addr->sin_addr.s_addr);

        freeaddrinfo(info);

        return result;
    }
}
