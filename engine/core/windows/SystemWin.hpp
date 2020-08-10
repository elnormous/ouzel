// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_SYSTEMWIN_HPP
#define OUZEL_CORE_SYSTEMWIN_HPP

#include "../System.hpp"

namespace ouzel::core::windows
{
    class System final: public core::System
    {
    public:
        System(int argc, LPWSTR* argv);
        ~System() override = default;
    };
}

#endif // OUZEL_CORE_SYSTEMWIN_HPP
