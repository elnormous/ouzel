// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "ColladaLoader.hpp"
#include "Bundle.hpp"
#include "scene/SkinnedMeshRenderer.hpp"
#include "utils/XML.hpp"

namespace ouzel
{
    namespace assets
    {
        ColladaLoader::ColladaLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool ColladaLoader::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            xml::Data colladaData(data);

            if (colladaData.getChildren().empty())
                throw std::runtime_error("Invalid Collada file");

            xml::Node rootNode = colladaData.getChildren().front();

            if (rootNode.getValue() != "COLLADA")
                throw std::runtime_error("Invalid Collada file");

            scene::SkinnedMeshData meshData;

            // TODO: load the model

            bundle.setSkinnedMeshData(filename, meshData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
