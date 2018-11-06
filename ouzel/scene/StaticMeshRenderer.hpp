// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef STATICMESHRENDERER_HPP
#define STATICMESHRENDERER_HPP

#include "scene/Component.hpp"
#include "scene/StaticMeshData.hpp"

namespace ouzel
{
    namespace scene
    {
        class StaticMeshRenderer: public Component
        {
        public:
            static const uint32_t CLASS = Component::MODEL_RENDERER;

            StaticMeshRenderer();
            explicit StaticMeshRenderer(const StaticMeshData& meshData);
            explicit StaticMeshRenderer(const std::string& filename);

            void init(const StaticMeshData& meshData);
            void init(const std::string& filename);

            void draw(const Matrix4& transformMatrix,
                      float opacity,
                      const Matrix4& renderViewProjection,
                      bool wireframe) override;

            inline const std::shared_ptr<graphics::Material>& getMaterial() const { return material; }
            inline void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

        private:
            std::shared_ptr<graphics::Material> material;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;

            std::shared_ptr<graphics::Texture> whitePixelTexture;
        };
    } // namespace scene
} // namespace ouzel

#endif // STATICMESHRENDERER_HPP
