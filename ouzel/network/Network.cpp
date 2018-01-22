// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include "Network.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace network
    {
        Network::Network()
        {
        }

        Network::~Network()
        {
            if (endpoint != NULL_SOCKET)
            {
#ifdef _WIN32
                int result = closesocket(endpoint);
#else
                int result = close(endpoint);
#endif

                if (result < 0)
                {
                    int error = getLastError();
                    Log(Log::Level::ERR) << "Failed to close socket, error: " << error;
                }
            }

#ifdef _WIN32
            WSACleanup();
#endif
        }

        int Network::getLastError()
        {
#ifdef _WIN32
            return WSAGetLastError();
#else
            return errno;
#endif
        }

        bool Network::getAddress(const std::string& address, uint32_t& result)
        {
            addrinfo* info;
            int ret = getaddrinfo(address.c_str(), nullptr, nullptr, &info);

            if (ret != 0)
            {
                int error = getLastError();
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
#ifdef _WIN32
            WORD sockVersion = MAKEWORD(2, 2);
            WSADATA wsaData;
            int error = WSAStartup(sockVersion, &wsaData);
            if (error != 0)
            {
                Log(Log::Level::ERR) << "Failed to start Winsock failed, error: " << error;
                return false;
            }

            if (wsaData.wVersion != sockVersion)
            {
                Log(Log::Level::ERR) << "Incorrect Winsock version";
                WSACleanup();
                return false;
            }
#endif

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
