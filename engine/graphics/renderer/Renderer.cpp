// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "Renderer.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Object.hpp"

namespace ouzel::graphics::renderer
{
    Renderer::Renderer(RenderDevice& initRenderDevice):
        renderDevice(initRenderDevice)
    {
    }

    void Renderer::addCommand(const Command& command)
    {
        std::lock_guard lock(commandQueueMutex);
        commandQueue.push(command);
    }

    void Renderer::render()
    {
        // TODO: sort objects by distance
        // TODO: render shadow map
        // TODO: render scene
        // TODO: post process
    }
}
