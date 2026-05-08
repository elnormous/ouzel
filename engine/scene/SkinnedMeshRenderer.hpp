#ifndef FLUXION_SCENE_SKINNEDMESHRENDERER_HPP
#define FLUXION_SCENE_SKINNEDMESHRENDERER_HPP

#include <memory>
#include <vector>
#include "Component.hpp"
#include "../graphics/Material.hpp"
#include "../reflection/Macros.hpp"

namespace fluxion::scene
{
    class SkinnedMeshData final
    {
    public:
        struct Bone final
        {
            Bone* parent = nullptr;
            math::Vector<float, 3> position{};
            math::Quaternion<float> rotation = math::identityQuaternion<float>;
        };

        SkinnedMeshData() = default;
        SkinnedMeshData(const math::Box<float, 3>& initBoundingBox,
                        const std::shared_ptr<graphics::Material>& initMaterial):
            boundingBox{initBoundingBox},
            material{initMaterial}
        {
        }

        math::Box<float, 3> boundingBox;
        std::shared_ptr<graphics::Material> material;
    };

    class SkinnedMeshRenderer: public Component
    {
    public:
        FLUXION_COMPONENT_DECL(SkinnedMeshRenderer, "scene.SkinnedMeshRenderer")

        SkinnedMeshRenderer();
        explicit SkinnedMeshRenderer(const SkinnedMeshData& meshData);

        void init(const SkinnedMeshData& meshData);

        void draw(const math::Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const math::Matrix<float, 4>& renderViewProjection,
                  bool wireframe) override;

        auto& getMaterial() const noexcept { return material; }
        void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

        void inspect(reflection::Inspector& inspector) override;

    private:
        std::shared_ptr<graphics::Material> material;
    };
}

#endif // FLUXION_SCENE_SKINNEDMESHRENDERER_HPP
