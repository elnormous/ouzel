// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BLENDFACTOR_HPP
#define OUZEL_GRAPHICS_BLENDFACTOR_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class BlendFactor
        {
            Zero,
            One,
            SrcColor,
            InvSrcColor,
            SrcAlpha,
            InvSrcAlpha,
            DestAlpha,
            InvDestAlpha,
            DestColor,
            InvDestColor,
            SrcAlphaSat,
            BlendFactor,
            InvBlendFactor
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BLENDFACTOR_HPP
