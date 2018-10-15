// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SkinnedMeshData.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        SkinnedMeshData::SkinnedMeshData(Box3 newBoundingBox,
                                         const std::shared_ptr<graphics::Material>& newMaterial)
        {
            boundingBox = newBoundingBox;

            material = newMaterial;
        }
    } // namespace scene
} // namespace ouzel
