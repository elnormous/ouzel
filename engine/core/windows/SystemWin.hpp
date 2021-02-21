// Ouzel by Elviss Strazdins

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
