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
            Loader(TYPE, {"dae"})
        {
        }

        bool LoaderCollada::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            xml::Data colladaData;

            if (!colladaData.init(data)) return false;

            scene::ModelData modelData;

            // TODO: load the model

            engine->getCache()->setModelData(filename, modelData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
