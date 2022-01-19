// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_LIGHT_HPP
#define OUZEL_SCENE_LIGHT_HPP

#include "Component.hpp"
#include "../math/Color.hpp"
#include "../math/Quaternion.hpp"

namespace ouzel::scene
{
    class Light: public Component
    {
    public:
        enum class Type
        {
            point,
            spot,
            directional
        };

        Light() = default;
        explicit Light(Type initType);
        ~Light() override;

        auto getType() const noexcept { return type; }
        void setType(Type newType) { type = newType; }

        auto getColor() const noexcept { return color; }
        void setColor(math::Color newColor) { color = newColor; }

        auto& getDirection() const noexcept { return direction; }
        void setDirection(const math::Quaternion<float>& newDirection) { direction = newDirection; }

        auto getAngle() const noexcept { return angle; }
        void setAngle(float newAngle) { angle = newAngle; }

        auto getRange() const noexcept { return range; }
        void setRange(float newRange) { range = newRange; }

        auto getIntensity() const noexcept { return intensity; }
        void setIntensity(float newIntensity) { intensity = newIntensity; }

    private:
        void setLayer(Layer* newLayer) override;

        Type type = Type::point;
        math::Color color = math::whiteColor;
        math::Quaternion<float> direction = math::identityQuaternion<float>; // for spot and directional light
        float angle = 0.0F; // for spot light
        float range = 0.0F; // for point and spot light
        float intensity = 1.0F;
    };
}

#endif // OUZEL_SCENE_LIGHT_HPP
