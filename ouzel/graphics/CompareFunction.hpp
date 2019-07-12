// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COMPAREFUNCTION_HPP
#define OUZEL_GRAPHICS_COMPAREFUNCTION_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class CompareFunction
        {
            NeverPass,
            PassIfLess,
            PassIfEqual,
            PassIfLessEqual,
            PassIfGreater,
            PassIfNotEqual,
            PassIfGreaterEqual,
            AlwaysPass
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_COMPAREFUNCTION_HPP
