// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GraphicsResource.hpp"
#include "Renderer.hpp"

namespace ouzel
{
    namespace graphics
    {
        Resource::Resource(Renderer& initRenderer):
            renderer(&initRenderer),
            id(renderer->getDevice()->getResourceId())
        {
        }

        Resource::~Resource()
        {
            if (id)
            {
                renderer->addCommand(std::unique_ptr<Command>(new DeleteResourceCommand(id)));
                RenderDevice* renderDevice = renderer->getDevice();
                renderDevice->deleteResourceId(id);
            }
        }
    } // namespace graphics
} // namespace ouzel
