// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SkinnedMeshRenderer.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        SkinnedMeshRenderer::SkinnedMeshRenderer():
            Component(CLASS)
        {
            whitePixelTexture = engine->getCache().getTexture(TEXTURE_WHITE_PIXEL);
        }

        SkinnedMeshRenderer::SkinnedMeshRenderer(const SkinnedMeshData& meshData):
            Component(CLASS)
        {
            init(meshData);
        }

        SkinnedMeshRenderer::SkinnedMeshRenderer(const std::string& filename):
            Component(CLASS)
        {
            init(filename);
        }

        void SkinnedMeshRenderer::init(const SkinnedMeshData& meshData)
        {
            boundingBox = meshData.boundingBox;
            material = meshData.material;
        }

        void SkinnedMeshRenderer::init(const std::string& filename)
        {
            init(*engine->getCache().getSkinnedMeshData(filename));
        }

        void SkinnedMeshRenderer::draw(const Matrix4<float>& transformMatrix,
                                       float opacity,
                                       const Matrix4<float>& renderViewProjection,
                                       bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);
        }
    } // namespace scene
} // namespace ouzel
