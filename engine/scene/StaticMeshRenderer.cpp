// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <limits>
#include "StaticMeshRenderer.hpp"
#include "../core/Engine.hpp"
#include "../utils/Utils.hpp"

namespace ouzel::scene
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

        for (const auto index : indices)
            if (index > std::numeric_limits<std::uint16_t>::max())
            {
                indexSize = sizeof(std::uint32_t);
                break;
            }

        if (indexSize == sizeof(std::uint16_t))
        {
            std::vector<std::uint16_t> convertedIndices;
            convertedIndices.reserve(indices.size());

            for (const auto index : indices)
                convertedIndices.push_back(static_cast<std::uint16_t>(index));

            indexBuffer = graphics::Buffer(*engine->getGraphics(),
                                           graphics::BufferType::index,
                                           graphics::Flags::none,
                                           convertedIndices.data(),
                                           static_cast<std::uint32_t>(getVectorSize(convertedIndices)));
        }
        else if (indexSize == sizeof(std::uint32_t))
            indexBuffer = graphics::Buffer(*engine->getGraphics(),
                                           graphics::BufferType::index,
                                           graphics::Flags::none,
                                           indices.data(),
                                           static_cast<std::uint32_t>(getVectorSize(indices)));

        vertexBuffer = graphics::Buffer(*engine->getGraphics(),
                                        graphics::BufferType::vertex,
                                        graphics::Flags::none,
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

        const auto modelViewProj = renderViewProjection * transformMatrix;
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

        std::vector<std::size_t> textures;
        for (const std::shared_ptr<graphics::Texture>& texture : material->textures)
            textures.push_back(texture ? texture->getResource() : 0);

        engine->getGraphics()->setPipelineState(material->blendState->getResource(),
                                                material->shader->getResource(),
                                                material->cullMode,
                                                wireframe ? graphics::FillMode::wireframe : graphics::FillMode::solid);
        engine->getGraphics()->setShaderConstants(fragmentShaderConstants,
                                                  vertexShaderConstants);
        engine->getGraphics()->setTextures(textures);
        engine->getGraphics()->draw(indexBuffer->getResource(),
                                    indexCount,
                                    indexSize,
                                    vertexBuffer->getResource(),
                                    graphics::DrawMode::triangleList,
                                    0);
    }
}
