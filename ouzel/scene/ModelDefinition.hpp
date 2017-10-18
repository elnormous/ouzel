// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "graphics/Buffer.hpp"
#include "graphics/Material.hpp"
#include "graphics/MeshBuffer.hpp"

namespace ouzel
{
    namespace scene
    {
        struct ModelDefinition
        {
        public:
            static ModelDefinition load(const std::string& filename, bool mipmaps = true);

        protected:
            Box3 boundingBox;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };
    } // namespace scene
} // namespace ouzel
