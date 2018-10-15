// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "scene/Component.hpp"
#include "scene/SkinnedMeshData.hpp"

namespace ouzel
{
    namespace scene
    {
        class SkinnedMeshRenderer: public Component
        {
        public:
            static const uint32_t CLASS = Component::MODEL_RENDERER;

            SkinnedMeshRenderer();
            explicit SkinnedMeshRenderer(const SkinnedMeshData& meshData);
            explicit SkinnedMeshRenderer(const std::string& filename);

            void init(const SkinnedMeshData& meshData);
            void init(const std::string& filename);

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              bool wireframe) override;

            virtual const std::shared_ptr<graphics::Material>& getMaterial() const { return material; }
            virtual void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

        private:
            std::shared_ptr<graphics::Material> material;
            std::shared_ptr<graphics::Texture> whitePixelTexture;
        };
    } // namespace scene
} // namespace ouzel
