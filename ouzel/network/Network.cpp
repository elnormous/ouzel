// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <poll.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include "Network.hpp"
#include "Client.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace network
    {
        Network::Network()
        {
#ifdef _WIN32
            WORD sockVersion = MAKEWORD(2, 2);
            WSADATA wsaData;
            int error = WSAStartup(sockVersion, &wsaData);
            if (error != 0)
                throw std::runtime_error("Failed to start WinSock failed, error: " + std::to_string(error));

            if (wsaData.wVersion != sockVersion)
                throw std::runtime_error("Incorrect WinSock version");
#endif
        }

        Network::~Network()
        {
#ifdef _WIN32
            if (endpoint != INVALID_SOCKET)
                closesocket(endpoint);

            WSACleanup();
#else
            if (endpoint != -1)
                close(endpoint);
#endif
        }

        bool Network::getAddress(const std::string& address, uint32_t& result)
        {
            addrinfo* info;
            int ret = getaddrinfo(address.c_str(), nullptr, nullptr, &info);

            if (ret != 0)
            {
#ifdef _WIN32
                int error = WSAGetLastError();
#else
                int error = errno;
#endif
                Log(Log::Level::ERR) << "Failed to get address info of " << address << ", error: " << error;
                return false;
            }

            sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(info->ai_addr);
            result = ntohl(addr->sin_addr.s_addr);

            freeaddrinfo(info);

            return true;
        }

        bool Network::init()
        {
            return true;
        }

        bool Network::listen(const std::string& address, uint16_t port)
        {
            return true;
        }

        bool Network::connect(const std::string& address, uint16_t port)
        {
            return true;
        }

        bool Network::disconnect()
        {
            return true;
        }
    } // namespace network
} // namespace ouzel
