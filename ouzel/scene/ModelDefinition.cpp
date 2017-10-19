// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ModelDefinition.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        bool ModelDefinition::load(const std::string& filename, bool mipmaps, ModelDefinition& modelDefinition)
        {
            std::vector<uint8_t> data;

            if (!sharedEngine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return true;
        }
    } // namespace scene
} // namespace ouzel
