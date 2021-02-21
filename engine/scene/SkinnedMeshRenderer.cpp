// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "SkinnedMeshRenderer.hpp"
#include "../core/Engine.hpp"

namespace ouzel::scene
{
    SkinnedMeshRenderer::SkinnedMeshRenderer()
    {
        whitePixelTexture = engine->getCache().getTexture(textureWhitePixel);
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
}
