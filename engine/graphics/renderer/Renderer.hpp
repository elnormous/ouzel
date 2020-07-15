// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
#define OUZEL_GRAPHICS_RENDERER_RENDERER_HPP

#include <cstddef>
#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Object.hpp"
#include "../RenderDevice.hpp"

namespace ouzel::graphics::renderer
{
    class Renderer final
    {
    public:
        struct Command final
        {
            enum class Type
            {
                initCamera,
                initLight,
                initMaterial,
                initObject
            };

            Type type;
        };

        Renderer() = default;

        using ResourceId = std::size_t;
        class Resource final
        {
            Resource() noexcept = default;
            Resource(Renderer& initRenderer):
                renderer{&initRenderer}, id{initRenderer.createResource()} {}

            ~Resource()
            {
                if (renderer && id) renderer->destroyResource(id);
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

    private:
        ResourceId createResource() { return 0; }
        void destroyResource(ResourceId id) { (void)id; }

        void render();

        std::size_t lastResourceId = 0;
        std::set<std::size_t> deletedResourceIds;
    };
}

#endif // OUZEL_GRAPHICS_RENDERER_RENDERER_HPP
