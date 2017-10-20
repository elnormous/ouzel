// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"
#include "scene/ModelData.hpp"

namespace ouzel
{
    namespace scene
    {
        class ModelRenderer: public Component
        {
        public:
            static const uint32_t TYPE = Component::MODEL_RENDERER;

            ModelRenderer();
            ModelRenderer(const ModelData& modelData);
            ModelRenderer(const std::string& filename, bool mipmaps = true);

            bool init(const ModelData& modelData);
            bool init(const std::string& filename, bool mipmaps = true);
        };
    } // namespace scene
} // namespace ouzel
