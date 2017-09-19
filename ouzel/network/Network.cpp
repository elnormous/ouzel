// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
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
#ifdef _WIN32
            WSACleanup();
#endif
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
