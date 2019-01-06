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
        class StaticMeshData
        {
        public:
            StaticMeshData() {}
            StaticMeshData(Box3<float> initBoundingBox,
                           const std::vector<uint32_t> indices,
                           const std::vector<graphics::Vertex>& vertices,
                           graphics::Material* initMaterial);

            Box3<float> boundingBox;
            graphics::Material* material = nullptr;
            uint32_t indexCount = 0;
            uint32_t indexSize = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_STATICMESHDATA_HPP
