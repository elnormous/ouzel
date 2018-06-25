// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MeshRenderer.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        MeshRenderer::MeshRenderer():
            Component(CLASS)
        {
            whitePixelTexture = engine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);
        }

        MeshRenderer::MeshRenderer(const MeshData& meshData):
            Component(CLASS)
        {
            init(meshData);
        }

        MeshRenderer::MeshRenderer(const std::string& filename, bool mipmaps):
            Component(CLASS)
        {
            init(filename, mipmaps);
        }

        void MeshRenderer::init(const MeshData& meshData)
        {
            boundingBox = meshData.boundingBox;
            material = meshData.material;
            indexCount = meshData.indexCount;
            indexSize = meshData.indexSize;
            indexBuffer = meshData.indexBuffer;
            vertexBuffer = meshData.vertexBuffer;
        }

        void MeshRenderer::init(const std::string& filename, bool mipmaps)
        {
            //const MeshData& meshData = engine->getCache()->
        }

        void MeshRenderer::draw(const Matrix4& transformMatrix,
                                 float opacity,
                                 const Matrix4& renderViewProjection,
                                 bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);

            material->cullMode = graphics::Renderer::CullMode::NONE;

            Matrix4 modelViewProj = renderViewProjection * transformMatrix;
            float colorVector[] = {material->diffuseColor.normR(), material->diffuseColor.normG(), material->diffuseColor.normB(), material->diffuseColor.normA() * opacity * material->opacity};

            std::vector<std::vector<float>> fragmentShaderConstants(1);
            fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            std::vector<std::shared_ptr<graphics::Texture>> textures;
            if (wireframe) textures.push_back(whitePixelTexture);
            else textures.assign(std::begin(material->textures), std::end(material->textures));

            engine->getRenderer()->addSetCullModeCommad(material->cullMode);
            engine->getRenderer()->addSetPipelineStateCommand(material->blendState,
                                                              material->shader);
            engine->getRenderer()->addSetShaderConstantsCommand(fragmentShaderConstants,
                                                                vertexShaderConstants);
            engine->getRenderer()->addSetTexturesCommand(textures);
            engine->getRenderer()->addDrawCommand(indexBuffer,
                                                  indexCount,
                                                  indexSize,
                                                  vertexBuffer,
                                                  graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                  0);
        }
    } // namespace scene
} // namespace ouzel
