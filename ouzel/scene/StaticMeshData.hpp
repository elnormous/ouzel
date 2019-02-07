// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_STATICMESHDATA_HPP
#define OUZEL_SCENE_STATICMESHDATA_HPP

#include <vector>
#include "graphics/Buffer.hpp"
#include "graphics/Material.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace scene
    {
        class StaticMeshData final
        {
        public:
            StaticMeshData() {}
            StaticMeshData(Box3<float> initBoundingBox,
                           const std::vector<uint32_t> indices,
                           const std::vector<graphics::Vertex>& vertices,
                           const std::shared_ptr<graphics::Material>& initMaterial);

            Box3<float> boundingBox;
            std::shared_ptr<graphics::Material> material;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_STATICMESHDATA_HPP
