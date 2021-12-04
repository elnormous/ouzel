// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_GLTFLOADER_HPP
#define OUZEL_ASSETS_GLTFLOADER_HPP

#include "Bundle.hpp"
#include "../scene/SkinnedMeshRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    inline bool loadGltf(Cache&,
                         Bundle& bundle,
                         const std::string& name,
                         const std::vector<std::byte>& data,
                         const Asset::Options&)
    {
        const auto d = json::parse(data);

        [[maybe_unused]] const auto& nodesValue = d["nodes"];
        [[maybe_unused]] const auto& meshesValue = d["meshes"];

        scene::SkinnedMeshData skinnedMeshData;
        bundle.setSkinnedMeshData(name, std::move(skinnedMeshData));

        return true;
    }
}

#endif // OUZEL_ASSETS_GLTFLOADER_HPP
