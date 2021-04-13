// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALERROR_HPP
#define OUZEL_GRAPHICS_METALERROR_HPP

#include <stdexcept>

namespace ouzel::graphics::metal
{
    class Error final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_GRAPHICS_METALERROR_HPP
