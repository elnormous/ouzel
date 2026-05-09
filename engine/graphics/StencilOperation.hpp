#ifndef FLUXION_GRAPHICS_STENCILPERATION_HPP
#define FLUXION_GRAPHICS_STENCILPERATION_HPP

namespace fluxion::graphics
{
    enum class StencilOperation
    {
        keep,
        zero,
        replace,
        incrementClamp,
        decrementClamp,
        invert,
        incrementWrap,
        decrementWrap
    };
}

#endif // FLUXION_GRAPHICS_STENCILPERATION_HPP
