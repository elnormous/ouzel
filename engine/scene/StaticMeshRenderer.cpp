// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <limits>
#include "StaticMeshRenderer.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        StaticMeshData::StaticMeshData(const Box3F& initBoundingBox,
                                       const std::vector<std::uint32_t> indices,
                                       const std::vector<graphics::Vertex>& vertices,
                                       const graphics::Material* initMaterial):
            boundingBox(initBoundingBox),
            material(initMaterial)
        {
            indexCount = static_cast<std::uint32_t>(indices.size());

            indexSize = sizeof(std::uint16_t);

            for (const std::uint32_t index : indices)
                if (index > std::numeric_limits<std::uint16_t>::max())
                {
                    indexSize = sizeof(std::uint32_t);
                    break;
                }

            if (indexSize == sizeof(std::uint16_t))
            {
                std::vector<std::uint16_t> convertedIndices;
                convertedIndices.reserve(indices.size());

                for (const std::uint32_t index : indices)
                    convertedIndices.push_back(static_cast<std::uint16_t>(index));

                indexBuffer = graphics::Buffer(*engine->getRenderer(),
                                               graphics::BufferType::Index, 0,
                                               convertedIndices.data(),
                                               static_cast<std::uint32_t>(getVectorSize(convertedIndices)));
            }
            else if (indexSize == sizeof(std::uint32_t))
                indexBuffer = graphics::Buffer(*engine->getRenderer(),
                                               graphics::BufferType::Index, 0,
                                               indices.data(),
                                               static_cast<std::uint32_t>(getVectorSize(indices)));

            vertexBuffer = graphics::Buffer(*engine->getRenderer(),
                                            graphics::BufferType::Vertex, 0,
                                            vertices.data(),
                                            static_cast<std::uint32_t>(getVectorSize(vertices)));
        }

        StaticMeshRenderer::StaticMeshRenderer(const StaticMeshData& meshData)
        {
            init(meshData);
        }

        void StaticMeshRenderer::init(const StaticMeshData& meshData)
        {
            boundingBox = meshData.boundingBox;
            material = meshData.material;
            indexCount = meshData.indexCount;
            indexSize = meshData.indexSize;
            indexBuffer = &meshData.indexBuffer;
            vertexBuffer = &meshData.vertexBuffer;
        }

        void StaticMeshRenderer::draw(const Matrix4F& transformMatrix,
                                      float opacity,
                                      const Matrix4F& renderViewProjection,
                                      bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);

            const Matrix4F modelViewProj = renderViewProjection * transformMatrix;
            const float colorVector[] = {
                material->diffuseColor.normR(),
                material->diffuseColor.normG(),
                material->diffuseColor.normB(),
                material->diffuseColor.normA() * opacity * material->opacity
            };

            std::vector<std::vector<float>> fragmentShaderConstants(1);
            fragmentShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(modelViewProj.m), std::end(modelViewProj.m)};

            std::vector<std::uintptr_t> textures;
            for (const std::shared_ptr<graphics::Texture>& texture : material->textures)
                textures.push_back(texture ? texture->getResource() : 0);

            engine->getRenderer()->setPipelineState(material->blendState->getResource(),
                                                    material->shader->getResource(),
                                                    material->cullMode,
                                                    wireframe ? graphics::FillMode::Wireframe : graphics::FillMode::Solid);
            engine->getRenderer()->setShaderConstants(fragmentShaderConstants,
                                                      vertexShaderConstants);
            engine->getRenderer()->setTextures(textures);
            engine->getRenderer()->draw(indexBuffer->getResource(),
                                        indexCount,
                                        indexSize,
                                        vertexBuffer->getResource(),
                                        graphics::DrawMode::TriangleList,
                                        0);
        }
    } // namespace scene
} // namespace ouzel
