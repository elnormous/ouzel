// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
            void init(Box3 newBoundingBox,
                      const std::vector<uint32_t> indices,
                      const std::vector<graphics::Vertex>& vertices,
                      const std::shared_ptr<graphics::Material>& newMaterial);

            Box3 boundingBox;
            std::shared_ptr<graphics::Material> material;
            uint32_t indexCount;
            uint32_t indexSize;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel
