// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMEM_HPP
#define OUZEL_CORE_SYSTEMEM_HPP

#include "../System.hpp"

namespace ouzel::core::emscripten
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;
    };
}

#endif // OUZEL_CORE_SYSTEMEM_HPP
