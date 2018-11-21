// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SkinnedMeshData.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        SkinnedMeshData::SkinnedMeshData(Box3 initBoundingBox,
                                         const std::shared_ptr<graphics::Material>& initMaterial):
            boundingBox(initBoundingBox),
            material(initMaterial)
        {
        }
    } // namespace scene
} // namespace ouzel
