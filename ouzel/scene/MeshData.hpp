// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <vector>
#include "graphics/Buffer.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Material.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace scene
    {
        class MeshData
        {
        public:
            MeshData() {}
            MeshData(Box3 newBoundingBox,
                     const std::vector<uint32_t> indices,
                     const std::vector<graphics::Vertex>& vertices,
                     const std::shared_ptr<graphics::Material>& newMaterial);

            Box3 boundingBox;
            std::shared_ptr<graphics::Material> material;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel
