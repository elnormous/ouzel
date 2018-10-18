// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <vector>
#include "graphics/Material.hpp"

namespace ouzel
{
    namespace scene
    {
        class SkinnedMeshData
        {
        public:
            struct Bone
            {
                Bone* parent = nullptr;
                Vector3 position;
                Quaternion rotation;
            };

            SkinnedMeshData() {}
            SkinnedMeshData(Box3 newBoundingBox,
                            const std::shared_ptr<graphics::Material>& newMaterial);

            Box3 boundingBox;
            std::shared_ptr<graphics::Material> material;
        };
    } // namespace scene
} // namespace ouzel
