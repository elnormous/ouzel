// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_SKINNEDMESHRENDERER_HPP
#define OUZEL_SCENE_SKINNEDMESHRENDERER_HPP

#include <vector>
#include "../scene/Component.hpp"
#include "../graphics/Material.hpp"

namespace ouzel::scene
{
    class SkinnedMeshData final
    {
    public:
        struct Bone final
        {
            Bone* parent = nullptr;
            Vector<float, 3> position;
            Quaternion<float> rotation;
        };

        SkinnedMeshData() = default;
        SkinnedMeshData(const Box<float, 3>& initBoundingBox,
                        const std::shared_ptr<graphics::Material>& initMaterial):
            boundingBox(initBoundingBox),
            material(initMaterial)
        {
        }

        Box<float, 3> boundingBox;
        std::shared_ptr<graphics::Material> material;
    };

    class SkinnedMeshRenderer: public Component
    {
    public:
        SkinnedMeshRenderer();
        explicit SkinnedMeshRenderer(const SkinnedMeshData& meshData);

        void init(const SkinnedMeshData& meshData);

        void draw(const Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const Matrix<float, 4>& renderViewProjection,
                  bool wireframe) override;

        auto& getMaterial() const noexcept { return material; }
        void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

    private:
        std::shared_ptr<graphics::Material> material;
        std::shared_ptr<graphics::Texture> whitePixelTexture;
    };
}

#endif // OUZEL_SCENE_SKINNEDMESHRENDERER_HPP
