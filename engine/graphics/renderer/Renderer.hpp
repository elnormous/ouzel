// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
#define OUZEL_GRAPHICS_RENDERER_RENDERER_HPP

#include <cstddef>
#include <mutex>
#include <queue>
#include "../RenderDevice.hpp"

namespace ouzel::graphics::renderer
{
    class Renderer final
    {
    public:
        Renderer() = default;

        using ResourceId = std::size_t;

        class Resource final
        {
        public:
            Resource() noexcept = default;
            Resource(Renderer& initRenderer):
                renderer{&initRenderer}, id{initRenderer.createResourceId()} {}

            ~Resource()
            {
                if (renderer && id) renderer->deleteResourceId(id);
            }

            Resource(const Resource& other) = delete;
            Resource& operator=(const Resource& other) = delete;

            Resource(Resource&& other) noexcept:
                renderer{other.renderer}, id{other.id}
            {
                other.renderer = nullptr;
                other.id = 0;
            }

            Resource& operator=(Resource&& other) noexcept
            {
                if (&other == this) return *this;

                renderer = other.renderer;
                id = other.id;
                other.renderer = nullptr;
                other.id = 0;

                return *this;
            }

            operator ResourceId() const noexcept { return id; }

        private:
            Renderer* renderer = nullptr;
            ResourceId id = 0;
        };

        void addCommand(const Command& command);

    private:
        ResourceId createResourceId()
        {
            const auto i = deletedResourceIds.begin();

            if (i == deletedResourceIds.end())
                return ++lastResourceId;
            else
            {
                std::size_t resourceId = *i;
                deletedResourceIds.erase(i);
                return resourceId;
            }
        }

        void deleteResourceId(ResourceId id)
        {
            deletedResourceIds.insert(id);
        }

        void render();

        ResourceId lastResourceId = 0;
        std::set<ResourceId> deletedResourceIds;

        std::queue<Command> commandQueue;
        std::mutex commandQueueMutex;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
