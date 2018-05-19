// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ModelData.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        bool ModelData::init(Box3 newBoundingBox,
                             const std::vector<uint32_t> indices,
                             const std::vector<graphics::Vertex>& vertices,
                             const std::shared_ptr<graphics::Material>& newMaterial)
        {
            boundingBox = newBoundingBox;

            indexCount = static_cast<uint32_t>(indices.size());
            
            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, 0, indices.data(), static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, 0, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint32_t), indexBuffer, vertexBuffer);

            material = newMaterial;

            return true;
        }
    } // namespace scene
} // namespace ouzel
