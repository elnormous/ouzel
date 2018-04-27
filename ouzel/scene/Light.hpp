// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"

namespace ouzel
{
    namespace scene
    {
        class Light: public Component
        {
        public:
            static const uint32_t CLASS = Component::LIGHT;

            Light();
            virtual ~Light();

        protected:
            virtual void setLayer(Layer* newLayer) override;
        };
    } // namespace scene
} // namespace ouzel
