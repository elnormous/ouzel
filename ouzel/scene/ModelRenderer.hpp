// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"
#include "scene/ModelDefinition.hpp"

namespace ouzel
{
    namespace scene
    {
        class ModelRenderer: public Component
        {
        public:
            static const uint32_t TYPE = Component::MODEL_RENDERER;

            ModelRenderer();
            ModelRenderer(const ModelDefinition& modelDefinition);
            ModelRenderer(const std::string& filename, bool mipmaps = true);

            bool init(const ModelDefinition& modelDefinition);
            bool init(const std::string& filename, bool mipmaps = true);
        };
    } // namespace scene
} // namespace ouzel
