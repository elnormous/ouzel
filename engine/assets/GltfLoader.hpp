// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_GLTFLOADER_HPP
#define OUZEL_ASSETS_GLTFLOADER_HPP

#include "Loader.hpp"
#include "Bundle.hpp"
#include "../scene/SkinnedMeshRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    class GltfLoader final: public Loader
    {
    public:
        explicit GltfLoader(): Loader{Asset::Type::skinnedMesh} {}

        bool loadAsset(Cache&,
                       Bundle& bundle,
                       const std::string& name,
                       const std::vector<std::byte>& data,
                       const Asset::Options&) override
        {
            const auto d = json::parse(data);

            [[maybe_unused]] const auto& nodesValue = d["nodes"];
            [[maybe_unused]] const auto& meshesValue = d["meshes"];

            scene::SkinnedMeshData skinnedMeshData;
            bundle.setSkinnedMeshData(name, std::move(skinnedMeshData));

            return true;
        }
    };
}

#endif // OUZEL_ASSETS_GLTFLOADER_HPP
