// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "GltfLoader.hpp"
#include "Bundle.hpp"
#include "../scene/SkinnedMeshRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    GltfLoader::GltfLoader(Cache& initCache):
        Loader(initCache, Type::skinnedMesh)
    {
    }

    bool GltfLoader::loadAsset(Bundle& bundle,
                               const std::string& name,
                               const std::vector<std::byte>& data,
                               bool mipmaps)
    {
        const auto d = json::parse(data);

        const auto& nodesValue = d["nodes"];
        const auto& meshesValue = d["meshes"];

        (void)mipmaps;
        (void)nodesValue;
        (void)meshesValue;

        scene::SkinnedMeshData skinnedMeshData;
        bundle.setSkinnedMeshData(name, std::move(skinnedMeshData));

        return true;
    }
}
