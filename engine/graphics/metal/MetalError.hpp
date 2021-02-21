// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALERROR_HPP
#define OUZEL_GRAPHICS_METALERROR_HPP

#include <stdexcept>

namespace ouzel::graphics::metal
{
    class Error final: public std::runtime_error
    {
    public:
        explicit Error(const std::string& str): std::runtime_error(str) {}
        explicit Error(const char* str): std::runtime_error(str) {}
    };
}

#endif // OUZEL_GRAPHICS_METALERROR_HPP
