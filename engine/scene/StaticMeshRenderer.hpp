// Ouzel by Elviss Strazdins

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
        StaticMeshData(const Box<float, 3>& initBoundingBox,
                       const std::vector<std::uint32_t> indices,
                       const std::vector<graphics::Vertex>& vertices,
                       const graphics::Material* initMaterial);

        Box<float, 3> boundingBox;
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

        void draw(const Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const Matrix<float, 4>& renderViewProjection,
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
