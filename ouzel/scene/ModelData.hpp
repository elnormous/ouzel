// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "graphics/Buffer.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Material.hpp"
#include "graphics/MeshBuffer.hpp"

namespace ouzel
{
    namespace scene
    {
        struct ModelData
        {
        public:
            bool init(const std::string& filename, bool mipmaps = true);
            bool init(Box3 newBoundingBox,
                      const std::vector<uint32_t> indices,
                      const std::vector<graphics::Vertex>& vertices,
                      const std::shared_ptr<graphics::Material>& newMaterial);

            Box3 boundingBox;
            std::shared_ptr<graphics::Material> material;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel
