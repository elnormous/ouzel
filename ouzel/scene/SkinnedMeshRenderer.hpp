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

            SkinnedMeshData() {}
            SkinnedMeshData(Box3F initBoundingBox,
                            graphics::Material* initMaterial):
                boundingBox(initBoundingBox),
                material(initMaterial)
            {
            }

            Box3F boundingBox;
            graphics::Material* material = nullptr;
        };

        class SkinnedMeshRenderer: public Component
        {
        public:
            static constexpr uint32_t CLASS = Component::MODEL_RENDERER;

            SkinnedMeshRenderer();
            explicit SkinnedMeshRenderer(const SkinnedMeshData& meshData);
            explicit SkinnedMeshRenderer(const std::string& filename);

            void init(const SkinnedMeshData& meshData);
            void init(const std::string& filename);

            void draw(const Matrix4F& transformMatrix,
                      float opacity,
                      const Matrix4F& renderViewProjection,
                      bool wireframe) override;

            inline graphics::Material* getMaterial() const { return material; }
            inline void setMaterial(graphics::Material* newMaterial) { material = newMaterial; }

        private:
            graphics::Material* material = nullptr;
            graphics::Texture* whitePixelTexture = nullptr;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_SKINNEDMESHRENDERER_HPP
