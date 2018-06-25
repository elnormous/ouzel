// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

            enum class Type
            {
                POINT,
                SPOT,
                DIRECTIONAL
            };

            Light(Type initType);
            virtual ~Light();

        protected:
            virtual void setLayer(Layer* newLayer) override;

        private:
            Type type;
        };
    } // namespace scene
} // namespace ouzel
