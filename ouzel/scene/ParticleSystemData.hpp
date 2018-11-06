// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_PARTICLESYSTEMDATA_HPP
#define OUZEL_PARTICLESYSTEMDATA_HPP

#include <memory>
#include <string>
#include "math/Vector2.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        struct ParticleSystemData final
        {
            enum class EmitterType
            {
                GRAVITY,
                RADIUS
            };

            enum class PositionType
            {
                FREE,
                PARENT, // relative to parent
                GROUPED
            };

            std::string name;

            uint32_t blendFuncSource = 0;
            uint32_t blendFuncDestination = 0;

            EmitterType emitterType = EmitterType::GRAVITY;
            uint32_t maxParticles = 0;
            float duration = 0.0F;
            float particleLifespan = 0.0F;
            float particleLifespanVariance = 0.0F;

            float speed = 0.0F;
            float speedVariance = 0.0F;

            Vector2 sourcePosition;
            Vector2 sourcePositionVariance;

            PositionType positionType = PositionType::FREE;

            float startParticleSize = 0.0F;
            float startParticleSizeVariance = 0.0F;

            float finishParticleSize = 0.0F;
            float finishParticleSizeVariance = 0.0F;

            float angle = 0.0F;
            float angleVariance = 0.0F;

            float startRotation = 0.0F;
            float startRotationVariance = 0.0F;

            float finishRotation = 0.0F;
            float finishRotationVariance = 0.0F;

            float rotatePerSecond = 0.0F;
            float rotatePerSecondVariance = 0.0F;

            float minRadius = 0.0F;
            float minRadiusVariance = 0.0F;

            float maxRadius = 0.0F;
            float maxRadiusVariance = 0.0F;

            float radialAcceleration = 0.0F;
            float radialAccelVariance = 0.0F;

            float tangentialAcceleration = 0.0F;
            float tangentialAccelVariance = 0.0F;

            bool absolutePosition = false;
            bool yCoordFlipped = false;
            bool rotationIsDir = false;

            Vector2 gravity = Vector2(0.0F, 0.0F);

            float startColorRed = 0.0F;
            float startColorGreen = 0.0F;
            float startColorBlue = 0.0F;
            float startColorAlpha = 0.0F;

            float startColorRedVariance = 0.0F;
            float startColorGreenVariance = 0.0F;
            float startColorBlueVariance = 0.0F;
            float startColorAlphaVariance = 0.0F;

            float finishColorRed = 0.0F;
            float finishColorGreen = 0.0F;
            float finishColorBlue = 0.0F;
            float finishColorAlpha = 0.0F;

            float finishColorRedVariance = 0.0F;
            float finishColorGreenVariance = 0.0F;
            float finishColorBlueVariance = 0.0F;
            float finishColorAlphaVariance = 0.0F;

            float emissionRate = 0.0F;

            std::shared_ptr<graphics::Texture> texture;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_PARTICLESYSTEMDATA_HPP
