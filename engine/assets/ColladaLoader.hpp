// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_COLLADALOADER_HPP
#define OUZEL_ASSETS_COLLADALOADER_HPP

#include "AssetError.hpp"
#include "Bundle.hpp"
#include "../scene/SkinnedMeshRenderer.hpp"
#include "../formats/Xml.hpp"

namespace ouzel::assets
{
    inline bool loadCollada(Cache&,
                            Bundle& bundle,
                            const std::string& name,
                            const std::vector<std::byte>& data,
                            const Asset::Options&)
    {
        const auto colladaData = xml::parse(data);

        if (colladaData.getChildren().empty())
            throw AssetError{"Invalid Collada file"};

        const auto rootNode = colladaData.getChildren().front();

        if (rootNode.getValue() != "COLLADA")
            throw AssetError{"Invalid Collada file"};

        scene::SkinnedMeshData meshData;

        // TODO: load the model

        bundle.setSkinnedMeshData(name, std::move(meshData));

        return true;
    }
}

#endif // OUZEL_ASSETS_COLLADALOADER_HPP
