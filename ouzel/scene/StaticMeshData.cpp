// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "StaticMeshData.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        StaticMeshData::StaticMeshData(Box3 initBoundingBox,
                                       const std::vector<uint32_t> indices,
                                       const std::vector<graphics::Vertex>& vertices,
                                       const std::shared_ptr<graphics::Material>& initMaterial):
            boundingBox(initBoundingBox),
            material(initMaterial)
        {
            indexCount = static_cast<uint32_t>(indices.size());
            indexSize = sizeof(uint32_t);

            indexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer());
            indexBuffer->init(graphics::Buffer::Usage::INDEX, 0, indices.data(), static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer());
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, 0, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));
        }
    } // namespace scene
} // namespace ouzel
