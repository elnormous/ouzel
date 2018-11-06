// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERTARGETRESOURCEMETAL_HPP
#define RENDERTARGETRESOURCEMETAL_HPP

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

#endif // RENDERTARGETRESOURCEMETAL_HPP
