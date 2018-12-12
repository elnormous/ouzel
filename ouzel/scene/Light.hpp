// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_LIGHT_HPP
#define OUZEL_SCENE_LIGHT_HPP

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
            static constexpr uint32_t CLASS = Component::LIGHT;

            enum class Type
            {
                POINT,
                SPOT,
                DIRECTIONAL
            };

            Light();
            explicit Light(Type initType);
            virtual ~Light();

            inline Type getType() const { return type; }
            inline void setType(Type newType) { type = newType; }

            inline Color getColor() const { return color; }
            inline void setColor(Color newColor) { color = newColor; }

            inline const Quaternion& getDirection() const { return direction; }
            inline void setDirection(const Quaternion& newDirection) { direction = newDirection; }

            inline float getAngle() const { return angle; }
            inline void setAngle(float newAngle) { angle = newAngle; }

            inline float getRange() const { return range; }
            inline void setRange(float newRange) { range = newRange; }

            inline float getIntensity() const { return intensity; }
            inline void setIntensity(float newIntensity) { intensity = newIntensity; }

        protected:
            void setLayer(Layer* newLayer) override;

        private:
            Type type = Type::POINT;
            Color color;
            Quaternion direction = Quaternion::identity(); // for spot and directional ligt
            float angle = 0.0F; // for spot light
            float range = 0.0F; // for point and spot light
            float intensity = 1.0F;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_LIGHT_HPP
