// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_SKINNEDMESHDATA_HPP
#define OUZEL_SCENE_SKINNEDMESHDATA_HPP

#include <vector>
#include "graphics/Material.hpp"

namespace ouzel
{
    namespace scene
    {
        class SkinnedMeshData final
        {
        public:
            struct Bone final
            {
                Bone* parent = nullptr;
                Vector3<float> position;
                Quaternion<float> rotation;
            };

            SkinnedMeshData() {}
            SkinnedMeshData(Box3<float> initBoundingBox,
                            const std::shared_ptr<graphics::Material>& initMaterial);

            Box3<float> boundingBox;
            std::shared_ptr<graphics::Material> material;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_SKINNEDMESHDATA_HPP
