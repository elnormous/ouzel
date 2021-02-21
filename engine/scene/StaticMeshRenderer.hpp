// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_STATICMESHRENDERER_HPP
#define OUZEL_SCENE_STATICMESHRENDERER_HPP

#include <vector>
#include "Component.hpp"
#include "../graphics/Buffer.hpp"
#include "../graphics/Material.hpp"
#include "../graphics/Vertex.hpp"

namespace ouzel::scene
{
    class StaticMeshData final
    {
    public:
        StaticMeshData() = default;
        StaticMeshData(const Box3F& initBoundingBox,
                       const std::vector<std::uint32_t> indices,
                       const std::vector<graphics::Vertex>& vertices,
                       const graphics::Material* initMaterial);

        Box3F boundingBox;
        const graphics::Material* material = nullptr;
        std::uint32_t indexCount = 0;
        std::uint32_t indexSize = 0;
        graphics::Buffer indexBuffer;
        graphics::Buffer vertexBuffer;
    };

    class StaticMeshRenderer: public Component
    {
    public:
        StaticMeshRenderer() = default;
        explicit StaticMeshRenderer(const StaticMeshData& meshData);

        void init(const StaticMeshData& meshData);

        void draw(const Matrix4F& transformMatrix,
                  float opacity,
                  const Matrix4F& renderViewProjection,
                  bool wireframe) override;

        auto& getMaterial() const noexcept { return material; }
        void setMaterial(const graphics::Material* newMaterial)
        {
            material = newMaterial;
        }

    private:
        const graphics::Material* material = nullptr;
        std::uint32_t indexCount = 0;
        std::uint32_t indexSize = 0;
        const graphics::Buffer* indexBuffer = nullptr;
        const graphics::Buffer* vertexBuffer = nullptr;
    };
}

#endif // OUZEL_SCENE_STATICMESHRENDERER_HPP
