// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "StaticMeshData.hpp"
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
            indexSize = sizeof(uint32_t);

            indexBuffer.reset(new graphics::Buffer(*engine->getRenderer(),
                                                   graphics::Buffer::Usage::INDEX, 0,
                                                   indices.data(),
                                                   static_cast<uint32_t>(getVectorSize(indices))));

            vertexBuffer.reset(new graphics::Buffer(*engine->getRenderer(),
                                                    graphics::Buffer::Usage::VERTEX, 0,
                                                    vertices.data(),
                                                    static_cast<uint32_t>(getVectorSize(vertices))));
        }
    } // namespace scene
} // namespace ouzel
