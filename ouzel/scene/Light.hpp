// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "scene/Component.hpp"
#include "math/Color.hpp"
#include "math/Quaternion.hpp"

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

            Color getColor() const { return color; }
            void setColor(Color newColor) { color = newColor; }

            const Quaternion& getDirection() const { return direction; }
            void setDirection(const Quaternion& newDirection) { direction = newDirection; }

            float getRange() const { return range; }
            void setRange(float newRange) { range = newRange; }

        protected:
            virtual void setLayer(Layer* newLayer) override;

        private:
            Type type;
            Color color;
            Quaternion direction = Quaternion::identity();
            float range = 0.0F;
        };
    } // namespace scene
} // namespace ouzel
