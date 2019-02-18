// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_STATICMESHRENDERER_HPP
#define OUZEL_SCENE_STATICMESHRENDERER_HPP

#include <vector>
#include "scene/Component.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Material.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace scene
    {
        class StaticMeshData final
        {
        public:
            StaticMeshData() {}
            StaticMeshData(Box3<float> initBoundingBox,
                           const std::vector<uint32_t> indices,
                           const std::vector<graphics::Vertex>& vertices,
                           graphics::Material* initMaterial);

            Box3<float> boundingBox;
            graphics::Material* material = nullptr;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::unique_ptr<graphics::Buffer> indexBuffer;
            std::unique_ptr<graphics::Buffer> vertexBuffer;
        };

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
            graphics::Buffer* indexBuffer = nullptr;
            graphics::Buffer* vertexBuffer = nullptr;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_STATICMESHRENDERER_HPP
