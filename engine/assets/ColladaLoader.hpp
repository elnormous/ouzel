// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_COLLADALOADER_HPP
#define OUZEL_ASSETS_COLLADALOADER_HPP

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
            throw std::runtime_error("Invalid Collada file");

        const auto rootNode = colladaData.getChildren().front();

        if (rootNode.getValue() != "COLLADA")
            throw std::runtime_error("Invalid Collada file");

        scene::SkinnedMeshData meshData;

        // TODO: load the model

        bundle.setSkinnedMeshData(name, std::move(meshData));

        return true;
    }
}

#endif // OUZEL_ASSETS_COLLADALOADER_HPP
