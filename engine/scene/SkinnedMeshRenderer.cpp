// Ouzel by Elviss Strazdins

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

    void SkinnedMeshRenderer::draw(const Matrix<float, 4>& transformMatrix,
                                   float opacity,
                                   const Matrix<float, 4>& renderViewProjection,
                                   bool wireframe)
    {
        Component::draw(transformMatrix,
                        opacity,
                        renderViewProjection,
                        wireframe);
    }
}
