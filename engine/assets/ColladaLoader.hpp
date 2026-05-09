#ifndef FLUXION_ASSETS_COLLADALOADER_HPP
#define FLUXION_ASSETS_COLLADALOADER_HPP

#include "AssetError.hpp"
#include "Bundle.hpp"
#include "../scene/SkinnedMeshRenderer.hpp"
#include "../formats/Xml.hpp"

namespace fluxion::assets
{
    inline bool loadCollada(Cache&,
                            Bundle& bundle,
                            const Uuid& uuid,
                            const std::string& name,
                            const std::vector<std::byte>& data,
                            const Asset::Options&)
    {
        const auto colladaData = xml::parse(data);

        if (colladaData.getChildren().empty())
            throw Error{"Invalid Collada file"};

        const auto rootNode = colladaData.getChildren().front();

        if (rootNode.getValue() != "COLLADA")
            throw Error{"Invalid Collada file"};

        scene::SkinnedMeshData meshData;

        // TODO: load the model

        bundle.setSkinnedMeshData(name, std::move(meshData), uuid);

        return true;
    }
}

#endif // FLUXION_ASSETS_COLLADALOADER_HPP
