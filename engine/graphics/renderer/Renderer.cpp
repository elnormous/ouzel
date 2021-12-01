// Ouzel by Elviss Strazdins

#include "Renderer.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Object.hpp"

namespace ouzel::graphics::renderer
{
    Renderer::Renderer(RenderDevice& initRenderDevice):
        renderDevice{initRenderDevice}
    {
    }

    void Renderer::addCommand(const Command& command)
    {
        std::scoped_lock lock{commandQueueMutex};
        commandQueue.push(command);
    }

    void Renderer::render()
    {
        for (;;)
        {
            std::unique_lock lock{commandQueueMutex};
            if (commandQueue.empty()) break;
            Command command = std::move(commandQueue.front());
            commandQueue.pop();
            lock.unlock();

            switch (command.type)
            {
                case Command::Type::resize:
                {
                    // TODO: resize the render devices back buffer
                    break;
                }

                case Command::Type::initMaterial: break;
                case Command::Type::setMaterialParameter: break;
                case Command::Type::initObject: break;
                case Command::Type::setObjectParameters: break;
                case Command::Type::initCamera: break;
                case Command::Type::setCameraParameters: break;
                case Command::Type::initLight: break;
                case Command::Type::setLightParameters: break;
                case Command::Type::renderScene: break;
                default:
                    break;
            }
        }

        // TODO: sort objects by distance

        // render shadow map
        //for (const auto& light : lights) render light

        // render objects
        //for (const auto& object : objects) render object

        // TODO: post process
    }
}
