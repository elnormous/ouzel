// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COMPAREFUNCTION_HPP
#define OUZEL_GRAPHICS_COMPAREFUNCTION_HPP

namespace ouzel
{
    namespace graphics
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
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_COMPAREFUNCTION_HPP
