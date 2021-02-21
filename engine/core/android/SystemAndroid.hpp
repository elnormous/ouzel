// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_SYSTEMANDROID_HPP
#define OUZEL_CORE_SYSTEMANDROID_HPP

#include "../System.hpp"

namespace ouzel::core::android
{
    class System final: public core::System
    {
    public:
        System();
        ~System() override = default;
    };
}

#endif // OUZEL_CORE_SYSTEMANDROID_HPP
