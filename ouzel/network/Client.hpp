// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#ifdef _WIN32
#include <winsock.h>
#endif

namespace ouzel
{
    namespace network
    {
        class Client
        {
        public:
            ~Client() {}

            bool disconnect();

        private:
#ifdef _WIN32
            SOCKET endpoint = INVALID_SOCKET;
#else
            int endpoint = -1;
#endif
        };
    } // namespace network
} // namespace ouzel
