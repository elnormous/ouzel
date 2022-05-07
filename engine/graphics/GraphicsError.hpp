// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_GRAPHICSERROR_HPP
#define OUZEL_GRAPHICS_GRAPHICSERROR_HPP

#include <stdexcept>

namespace ouzel::grapchics
{
    class Error final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_GRAPHICS_GRAPHICSERROR_HPP
