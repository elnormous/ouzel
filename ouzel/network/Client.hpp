// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CLIENT_HPP
#define OUZEL_CLIENT_HPP

#include "Socket.hpp"

namespace ouzel
{
    namespace network
    {
        class Client final
        {
        public:
            ~Client();

            Client(const Client&) = delete;
            Client& operator=(const Client&) = delete;

            Client(Client&& other);
            Client& operator=(Client&& other);

            void disconnect();

        private:
            Socket sock;
        };
    } // namespace network
} // namespace ouzel

#endif // OUZEL_CLIENT_HPP
