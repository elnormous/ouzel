// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_SKINNEDMESHRENDERER_HPP
#define OUZEL_SCENE_SKINNEDMESHRENDERER_HPP

#include <vector>
#include "scene/Component.hpp"
#include "graphics/Material.hpp"

namespace ouzel
{
    namespace scene
    {
        class SkinnedMeshData final
        {
        public:
            struct Bone final
            {
                Bone* parent = nullptr;
                Vector3F position;
                QuaternionF rotation;
            };

            SkinnedMeshData() = default;
            SkinnedMeshData(const Box3F& initBoundingBox,
                            const std::shared_ptr<graphics::Material>& initMaterial):
                boundingBox(initBoundingBox),
                material(initMaterial)
            {
            }

            Box3F boundingBox;
            std::shared_ptr<graphics::Material> material;
        };

        class SkinnedMeshRenderer: public Component
        {
        public:
            SkinnedMeshRenderer();
            explicit SkinnedMeshRenderer(const SkinnedMeshData& meshData);

            void init(const SkinnedMeshData& meshData);

            void draw(const Matrix4F& transformMatrix,
                      float opacity,
                      const Matrix4F& renderViewProjection,
                      bool wireframe) override;

            inline const auto& getMaterial() const noexcept { return material; }
            inline void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

        private:
            std::shared_ptr<graphics::Material> material;
            std::shared_ptr<graphics::Texture> whitePixelTexture;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_SKINNEDMESHRENDERER_HPP
