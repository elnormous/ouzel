// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "MeshData.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        void MeshData::init(Box3 newBoundingBox,
                             const std::vector<uint32_t> indices,
                             const std::vector<graphics::Vertex>& vertices,
                             const std::shared_ptr<graphics::Material>& newMaterial)
        {
            boundingBox = newBoundingBox;

            indexCount = static_cast<uint32_t>(indices.size());
            indexSize = sizeof(uint32_t);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, 0, indices.data(), static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, 0, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));

            material = newMaterial;
        }
    } // namespace scene
} // namespace ouzel
