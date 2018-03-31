// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include "math/Vector2.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        struct ParticleSystemData
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

            uint32_t blendFuncSource = 1;
            uint32_t blendFuncDestination = 771;

            EmitterType emitterType = EmitterType::GRAVITY;
            uint32_t maxParticles = 77;
            float duration = -1.0F;
            float particleLifespan = 1.0F;
            float particleLifespanVariance = 0.0F;

            float speed = 225.0F;
            float speedVariance = 30.0F;

            Vector2 sourcePosition;
            Vector2 sourcePositionVariance;

            PositionType positionType = PositionType::FREE;

            float startParticleSize = 64.0F;
            float startParticleSizeVariance = 5.0F;

            float finishParticleSize = 0.0F;
            float finishParticleSizeVariance = 0.0F;

            float angle = 90.0F;
            float angleVariance = 10.0F;

            float startRotation = 0.0F;
            float startRotationVariance = 0.0F;

            float finishRotation = 0.0F;
            float finishRotationVariance = 0.0F;

            float rotatePerSecond = 360.0F;
            float rotatePerSecondVariance = 0.0F;

            float minRadius = 300.0F;
            float minRadiusVariance = 0.0F;

            float maxRadius = 0.0F;
            float maxRadiusVariance = 0.0F;

            float radialAcceleration = 0.0F;
            float radialAccelVariance = 0.0F;

            float tangentialAcceleration = 0.0F;
            float tangentialAccelVariance = 0.0F;

            bool absolutePosition = false;
            bool yCoordFlipped = true;
            bool rotationIsDir = false;

            Vector2 gravity = Vector2(0.0F, 0.0F);

            float startColorRed = 0.372F;
            float startColorGreen = 0.498F;
            float startColorBlue = 0.8F;
            float startColorAlpha = 0.5F;

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
