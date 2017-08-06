// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"

namespace ouzel
{
    namespace scene
    {
        class MeshRenderer: public Component
        {
        public:
            static const uint32_t TYPE = Component::MESH_RENDERER;

            MeshRenderer();
        };
    } // namespace scene
} // namespace ouzel
