// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <limits>
#include "StaticMeshRenderer.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        StaticMeshData::StaticMeshData(Box3<float> initBoundingBox,
                                       const std::vector<uint32_t> indices,
                                       const std::vector<graphics::Vertex>& vertices,
                                       graphics::Material* initMaterial):
            boundingBox(initBoundingBox),
            material(initMaterial)
        {
            indexCount = static_cast<uint32_t>(indices.size());
            indexSize = sizeof(uint16_t);

            for (uint32_t index : indices)
                if (index > std::numeric_limits<uint16_t>::max())
                {
                    indexSize = sizeof(uint32_t);
                    break;
                }

            if (indexSize == sizeof(uint16_t))
            {
                std::vector<uint16_t> convertedIndices;
                convertedIndices.reserve(indices.size());

                for (uint32_t index : indices)
                    convertedIndices.push_back(static_cast<uint16_t>(index));

                indexBuffer.reset(new graphics::Buffer(*engine->getRenderer(),
                                                       graphics::Buffer::Usage::INDEX, 0,
                                                       convertedIndices.data(),
                                                       static_cast<uint32_t>(getVectorSize(convertedIndices))));
            }
            else if (indexSize == sizeof(uint32_t))
                indexBuffer.reset(new graphics::Buffer(*engine->getRenderer(),
                                                       graphics::Buffer::Usage::INDEX, 0,
                                                       indices.data(),
                                                       static_cast<uint32_t>(getVectorSize(indices))));

            vertexBuffer.reset(new graphics::Buffer(*engine->getRenderer(),
                                                    graphics::Buffer::Usage::VERTEX, 0,
                                                    vertices.data(),
                                                    static_cast<uint32_t>(getVectorSize(vertices))));
        }

        StaticMeshRenderer::StaticMeshRenderer():
            Component(CLASS)
        {
        }

        StaticMeshRenderer::StaticMeshRenderer(const StaticMeshData& meshData):
            Component(CLASS)
        {
            init(meshData);
        }

        StaticMeshRenderer::StaticMeshRenderer(const std::string& filename):
            Component(CLASS)
        {
            init(filename);
        }

        void StaticMeshRenderer::init(const StaticMeshData& meshData)
        {
            boundingBox = meshData.boundingBox;
            material = meshData.material;
            indexCount = meshData.indexCount;
            indexSize = meshData.indexSize;
            indexBuffer = meshData.indexBuffer.get();
            vertexBuffer = meshData.vertexBuffer.get();
        }

        void StaticMeshRenderer::init(const std::string& filename)
        {
            init(*engine->getCache().getStaticMeshData(filename));
        }

        void StaticMeshRenderer::draw(const Matrix4<float>& transformMatrix,
                                      float opacity,
                                      const Matrix4<float>& renderViewProjection,
                                      bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);

            Matrix4<float> modelViewProj = renderViewProjection * transformMatrix;
            float colorVector[] = {material->diffuseColor.normR(), material->diffuseColor.normG(), material->diffuseColor.normB(), material->diffuseColor.normA() * opacity * material->opacity};

            std::vector<std::vector<float>> fragmentShaderConstants(1);
            fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            std::vector<uintptr_t> textures;
            for (graphics::Texture* texture : material->textures)
                textures.push_back(texture ? texture->getResource() : 0);

            engine->getRenderer()->setCullMode(material->cullMode);
            engine->getRenderer()->setPipelineState(material->blendState->getResource(),
                                                    material->shader->getResource());
            engine->getRenderer()->setShaderConstants(fragmentShaderConstants,
                                                      vertexShaderConstants);
            engine->getRenderer()->setTextures(textures);
            engine->getRenderer()->draw(indexBuffer->getResource(),
                                        indexCount,
                                        indexSize,
                                        vertexBuffer->getResource(),
                                        graphics::DrawMode::TRIANGLE_LIST,
                                        0);
        }
    } // namespace scene
} // namespace ouzel
