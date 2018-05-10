// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ModelRenderer.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        ModelRenderer::ModelRenderer():
            Component(CLASS)
        {
            whitePixelTexture = engine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);
        }

        ModelRenderer::ModelRenderer(const ModelData& modelData):
            Component(CLASS)
        {
            init(modelData);
        }

        ModelRenderer::ModelRenderer(const std::string& filename, bool mipmaps):
            Component(CLASS)
        {
            init(filename, mipmaps);
        }

        bool ModelRenderer::init(const ModelData& modelData)
        {
            boundingBox = modelData.boundingBox;
            material = modelData.material;
            meshBuffer = modelData.meshBuffer;
            indexBuffer = modelData.indexBuffer;
            vertexBuffer = modelData.vertexBuffer;

            return true;
        }

        bool ModelRenderer::init(const std::string& filename, bool mipmaps)
        {
            //const ModelData& modelData = engine->getCache()->
            return true;
        }

        void ModelRenderer::draw(const Matrix4& transformMatrix,
                                 float opacity,
                                 const Matrix4& renderViewProjection,
                                 bool depthWrite,
                                 bool depthTest,
                                 bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            depthWrite,
                            depthTest,
                            wireframe);

            material->cullMode = graphics::Renderer::CullMode::NONE;

            Matrix4 modelViewProj = renderViewProjection * transformMatrix;
            float colorVector[] = {material->diffuseColor.normR(), material->diffuseColor.normG(), material->diffuseColor.normB(), material->diffuseColor.normA() * opacity * material->opacity};

            std::vector<std::vector<float>> pixelShaderConstants(1);
            pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            std::vector<std::shared_ptr<graphics::Texture>> textures;
            if (wireframe) textures.push_back(whitePixelTexture);
            else textures.assign(std::begin(material->textures), std::end(material->textures));

            engine->getRenderer()->addSetCullModeCommad(material->cullMode);

            engine->getRenderer()->addDrawCommand(textures,
                                                  material->shader,
                                                  pixelShaderConstants,
                                                  vertexShaderConstants,
                                                  material->blendState,
                                                  meshBuffer,
                                                  0,
                                                  graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                  0,
                                                  depthWrite,
                                                  depthTest);
        }
    } // namespace scene
} // namespace ouzel
