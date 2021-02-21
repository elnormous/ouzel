// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_COMPAREFUNCTION_HPP
#define OUZEL_GRAPHICS_COMPAREFUNCTION_HPP

namespace ouzel::graphics
{
    enum class CompareFunction
    {
        never,
        less,
        equal,
        lessEqual,
        greater,
        notEqual,
        greaterEqual,
        always
    };
}

#endif // OUZEL_GRAPHICS_COMPAREFUNCTION_HPP
