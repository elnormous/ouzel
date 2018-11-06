// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_RENDERTARGETRESOURCEMETAL_HPP
#define OUZEL_RENDERTARGETRESOURCEMETAL_HPP

#include "graphics/metal/RenderResourceMetal.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderTargetResourceMetal final: public RenderResourceMetal
        {
        public:
            explicit RenderTargetResourceMetal(RenderDeviceMetal& renderDeviceMetal);
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_RENDERTARGETRESOURCEMETAL_HPP
