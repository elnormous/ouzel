// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "scene/Component.hpp"
#include "scene/MeshData.hpp"

namespace ouzel
{
    namespace scene
    {
        class MeshRenderer: public Component
        {
        public:
            static const uint32_t CLASS = Component::MODEL_RENDERER;

            MeshRenderer();
            explicit MeshRenderer(const MeshData& meshData);
            explicit MeshRenderer(const std::string& filename, bool mipmaps = true);

            void init(const MeshData& meshData);
            void init(const std::string& filename, bool mipmaps = true);

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              bool wireframe) override;

            virtual const std::shared_ptr<graphics::Material>& getMaterial() const { return material; }
            virtual void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

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
