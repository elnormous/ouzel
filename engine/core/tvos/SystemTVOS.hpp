// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_SYSTEMTVOS_HPP
#define OUZEL_CORE_SYSTEMTVOS_HPP

#include "../System.hpp"

namespace ouzel::core::tvos
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() = default;
    };
}

#endif // OUZEL_CORE_SYSTEMTVOS_HPP
