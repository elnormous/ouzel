// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_STATICMESHRENDERER_HPP
#define OUZEL_SCENE_STATICMESHRENDERER_HPP

#include "scene/Component.hpp"
#include "scene/StaticMeshData.hpp"

namespace ouzel
{
    namespace scene
    {
        class StaticMeshRenderer: public Component
        {
        public:
            static constexpr uint32_t CLASS = Component::MODEL_RENDERER;

            StaticMeshRenderer();
            explicit StaticMeshRenderer(const StaticMeshData& meshData);
            explicit StaticMeshRenderer(const std::string& filename);

            void init(const StaticMeshData& meshData);
            void init(const std::string& filename);

            void draw(const Matrix4<float>& transformMatrix,
                      float opacity,
                      const Matrix4<float>& renderViewProjection,
                      bool wireframe) override;

            inline graphics::Material* getMaterial() const { return material; }
            inline void setMaterial(graphics::Material* newMaterial) { material = newMaterial; }

        private:
            graphics::Material* material = nullptr;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_STATICMESHRENDERER_HPP
