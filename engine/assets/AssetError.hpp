// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_ASSETERROR_HPP
#define OUZEL_ASSETS_ASSETERROR_HPP

#include <stdexcept>

namespace ouzel::assets
{
    class Error final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_ASSETS_ASSETERROR_HPP
