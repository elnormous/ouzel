// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "ShaderResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        ShaderResource::ShaderResource(RenderDevice& initRenderDevice):
            renderDevice(initRenderDevice)
        {
        }

        ShaderResource::~ShaderResource()
        {
        }
    } // namespace graphics
} // namespace ouzel
