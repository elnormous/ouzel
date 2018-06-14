// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderCollada.hpp"
#include "core/Engine.hpp"
#include "scene/MeshData.hpp"
#include "utils/Log.hpp"
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
            xml::Data colladaData;
            colladaData.init(data);

            if (colladaData.getChildren().empty())
            {
                Log(Log::Level::ERR) << "Invalid collada Collada file";
                return false;
            }

            xml::Node rootNode = colladaData.getChildren().front();

            if (rootNode.getValue() != "COLLADA")
            {
                Log(Log::Level::ERR) << "Invalid collada Collada file";
                return false;
            }

            scene::MeshData meshData;

            // TODO: load the model

            engine->getCache()->setMeshData(filename, meshData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
