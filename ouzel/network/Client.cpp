// Copyright 2015-2018 Elviss Strazdins.
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
#include <unistd.h>
#endif
#include "Client.hpp"

namespace ouzel
{
    namespace network
    {
        Client::~Client()
        {
#ifdef _WIN32
            if (endpoint != INVALID_SOCKET) closesocket(endpoint);
#else
            if (endpoint != -1) close(endpoint);
#endif
        }

        Client::Client(Client&& other)
        {
            endpoint = other.endpoint;

#ifdef _WIN32
            other.endpoint = INVALID_SOCKET;
#else
            other.endpoint = -1;
#endif
        }

        Client& Client::operator=(Client&& other)
        {
            if (&other != this)
            {
#ifdef _WIN32
                if (endpoint != INVALID_SOCKET) closesocket(endpoint);
#else
                if (endpoint != -1) close(endpoint);
#endif

                endpoint = other.endpoint;

#ifdef _WIN32
                other.endpoint = INVALID_SOCKET;
#else
                other.endpoint = -1;
#endif
            }

            return *this;
        }

        bool Client::disconnect()
        {
            return true;
        }
    } // namespace network
} // namespace ouzel
