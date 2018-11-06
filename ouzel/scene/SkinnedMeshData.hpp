// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef SKINNEDMESHDATA_HPP
#define SKINNEDMESHDATA_HPP

#include <vector>
#include "graphics/Material.hpp"

namespace ouzel
{
    namespace scene
    {
        class SkinnedMeshData
        {
        public:
            struct Bone final
            {
                Bone* parent = nullptr;
                Vector3 position;
                Quaternion rotation;
            };

            SkinnedMeshData() {}
            SkinnedMeshData(Box3 initBoundingBox,
                            const std::shared_ptr<graphics::Material>& initMaterial);

            Box3 boundingBox;
            std::shared_ptr<graphics::Material> material;
        };
    } // namespace scene
} // namespace ouzel

#endif // SKINNEDMESHDATA_HPP
