#ifndef FLUXION_GRAPHICS_COMPAREFUNCTION_HPP
#define FLUXION_GRAPHICS_COMPAREFUNCTION_HPP

namespace fluxion::graphics
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

#endif // FLUXION_GRAPHICS_COMPAREFUNCTION_HPP
