// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef STATICMESHDATA_HPP
#define STATICMESHDATA_HPP

#include <vector>
#include "graphics/Buffer.hpp"
#include "graphics/Material.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace scene
    {
        class StaticMeshData
        {
        public:
            StaticMeshData() {}
            StaticMeshData(Box3 initBoundingBox,
                           const std::vector<uint32_t> indices,
                           const std::vector<graphics::Vertex>& vertices,
                           const std::shared_ptr<graphics::Material>& initMaterial);

            Box3 boundingBox;
            std::shared_ptr<graphics::Material> material;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel

#endif // STATICMESHDATA_HPP
