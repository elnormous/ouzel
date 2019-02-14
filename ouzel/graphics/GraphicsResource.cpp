// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "GraphicsResource.hpp"
#include "Renderer.hpp"

namespace ouzel
{
    namespace graphics
    {
        Resource::Resource(Renderer& initRenderer):
            renderer(&initRenderer),
            id(renderer->getResourceId())
        {
        }

        Resource::~Resource()
        {
            if (id) renderer->deleteResourceId(id);
        }
    } // namespace graphics
} // namespace ouzel
