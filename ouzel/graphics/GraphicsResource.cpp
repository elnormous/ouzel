// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "GraphicsResource.hpp"
#include "RenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        Resource::Resource(RenderDevice& initRendererDevice):
            renderDevice(&initRendererDevice),
            id(renderDevice->getResourceId())
        {
        }

        Resource::~Resource()
        {
            if (id) renderDevice->deleteResourceId(id);
        }
    } // namespace graphics
} // namespace ouzel
