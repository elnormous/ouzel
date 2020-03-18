// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "SkinnedMeshRenderer.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        SkinnedMeshRenderer::SkinnedMeshRenderer()
        {
            whitePixelTexture = engine->getCache().getTexture(TEXTURE_WHITE_PIXEL);
        }

        SkinnedMeshRenderer::SkinnedMeshRenderer(const SkinnedMeshData& meshData)
        {
            init(meshData);
        }

        void SkinnedMeshRenderer::init(const SkinnedMeshData& meshData)
        {
            boundingBox = meshData.boundingBox;
            material = meshData.material;
        }

        void SkinnedMeshRenderer::draw(const Matrix4F& transformMatrix,
                                       float opacity,
                                       const Matrix4F& renderViewProjection,
                                       bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);
        }
    } // namespace scene
} // namespace ouzel
