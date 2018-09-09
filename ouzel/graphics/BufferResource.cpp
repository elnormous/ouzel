// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "BufferResource.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace graphics
    {
        BufferResource::BufferResource(RenderDevice& initRenderDevice):
            RenderResource(initRenderDevice)
        {
        }

        BufferResource::~BufferResource()
        {
        }
    } // namespace graphics
} // namespace ouzel
