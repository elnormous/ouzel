// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "LoaderCollada.hpp"
#include "core/Engine.hpp"
#include "scene/ModelData.hpp"
#include "utils/XML.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderCollada::LoaderCollada():
            Loader({"dae"})
        {
        }

        bool LoaderCollada::loadAsset(const std::string& filename, const std::vector<uint8_t>& data)
        {
            xml::Data colladaData;

            if (!colladaData.init(data)) return false;

            scene::ModelData modelData;
            //modelData.init(boundingBox, indices, vertices, material);
            engine->getCache()->setModelData(filename, modelData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
