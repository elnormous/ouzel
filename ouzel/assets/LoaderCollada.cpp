// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderCollada.hpp"
#include "Cache.hpp"
#include "scene/MeshData.hpp"
#include "utils/Errors.hpp"
#include "utils/XML.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderCollada::LoaderCollada():
            Loader(TYPE, {"dae"})
        {
        }

        bool LoaderCollada::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            xml::Data colladaData(data);

            if (colladaData.getChildren().empty())
                throw ParseError("Invalid collada Collada file");

            xml::Node rootNode = colladaData.getChildren().front();

            if (rootNode.getValue() != "COLLADA")
                throw ParseError("Invalid collada Collada file");

            scene::MeshData meshData;

            // TODO: load the model

            cache->setMeshData(filename, meshData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
