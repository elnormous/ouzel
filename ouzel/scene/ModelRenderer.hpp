// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"

namespace ouzel
{
    namespace scene
    {
        class ModelRenderer: public Component
        {
        public:
            static const uint32_t TYPE = Component::MODEL_RENDERER;

            ModelRenderer();
        };
    } // namespace scene
} // namespace ouzel
