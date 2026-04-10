#ifndef FLUXION_CORE_SYSTEMANDROID_HPP
#define FLUXION_CORE_SYSTEMANDROID_HPP

#include "../System.hpp"

namespace fluxion::core::android
{
    class System final: public core::System
    {
    public:
        System();
        ~System() override = default;
    };
}

#endif // FLUXION_CORE_SYSTEMANDROID_HPP
