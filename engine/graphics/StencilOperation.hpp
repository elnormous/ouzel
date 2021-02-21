// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_STENCILPERATION_HPP
#define OUZEL_GRAPHICS_STENCILPERATION_HPP

namespace ouzel::graphics
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

#endif // OUZEL_GRAPHICS_STENCILPERATION_HPP
