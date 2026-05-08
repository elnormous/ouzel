#ifndef FLUXION_SCENE_STATICMESHRENDERER_HPP
#define FLUXION_SCENE_STATICMESHRENDERER_HPP

#include <vector>
#include "Component.hpp"
#include "../graphics/Buffer.hpp"
#include "../graphics/Material.hpp"
#include "../graphics/Vertex.hpp"
#include "../reflection/Macros.hpp"

namespace fluxion::scene
{
    class StaticMeshData final
    {
    public:
        StaticMeshData() = default;
        StaticMeshData(const math::Box<float, 3>& initBoundingBox,
                       const std::vector<std::uint32_t> indices,
                       const std::vector<graphics::Vertex>& vertices,
                       const graphics::Material* initMaterial);

        math::Box<float, 3> boundingBox;
        const graphics::Material* material = nullptr;
        std::uint32_t indexCount = 0;
        std::uint32_t indexSize = 0;
        graphics::Buffer indexBuffer;
        graphics::Buffer vertexBuffer;
    };

    class StaticMeshRenderer: public Component
    {
    public:
        FLUXION_COMPONENT_DECL(StaticMeshRenderer, "scene.StaticMeshRenderer")

        StaticMeshRenderer() = default;
        explicit StaticMeshRenderer(const StaticMeshData& meshData);

        void init(const StaticMeshData& meshData);

        void draw(const math::Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const math::Matrix<float, 4>& renderViewProjection,
                  bool wireframe) override;

        auto& getMaterial() const noexcept { return material; }
        void setMaterial(const graphics::Material* newMaterial)
        {
            material = newMaterial;
        }

        void inspect(reflection::Inspector& inspector) override;

    private:
        const graphics::Material* material = nullptr;
        std::uint32_t indexCount = 0;
        std::uint32_t indexSize = 0;
        const graphics::Buffer* indexBuffer = nullptr;
        const graphics::Buffer* vertexBuffer = nullptr;
    };
}

#endif // FLUXION_SCENE_STATICMESHRENDERER_HPP
