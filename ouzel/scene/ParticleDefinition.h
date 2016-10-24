// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "utils/Types.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        struct ParticleDefinition
        {
            static ParticleDefinition loadParticleDefinition(const std::string& filename);

            enum class EmitterType
            {
                GRAVITY,
                RADIUS,
            };

            enum class PositionType
            {
                FREE,
                RELATIVE,
                GROUPED,
            };

            uint32_t blendFuncSource = 1;
            uint32_t blendFuncDestination = 771;

            EmitterType emitterType = EmitterType::GRAVITY;
            uint32_t maxParticles = 77;
            float duration = -1.0f;
            float particleLifespan = 1.0f;
            float particleLifespanVariance = 0.0f;

            float speed = 225.0f;
            float speedVariance = 30.0f;

            Vector2 sourcePosition;
            Vector2 sourcePositionVariance;

            PositionType positionType = PositionType::FREE;

            float startParticleSize = 64.0f;
            float startParticleSizeVariance = 5.0f;

            float finishParticleSize = 0.0f;
            float finishParticleSizeVariance = 0.0f;

            float angle = 90.0f;
            float angleVariance = 10.0f;

            float startRotation = 0.0f;
            float startRotationVariance = 0.0f;

            float finishRotation = 0.0f;
            float finishRotationVariance = 0.0f;

            float rotatePerSecond = 360.0f;
            float rotatePerSecondVariance = 0.0f;

            float minRadius = 300.0f;
            float minRadiusVariance = 0.0f;

            float maxRadius = 0.0f;
            float maxRadiusVariance = 0.0f;

            float radialAcceleration = 0.0f;
            float radialAccelVariance = 0.0f;

            float tangentialAcceleration = 0.0f;
            float tangentialAccelVariance = 0.0f;

            bool absolutePosition = false;
            bool yCoordFlipped = true;
            bool rotationIsDir = false;

            Vector2 gravity = Vector2(0.0f, 0.0f);

            float startColorRed = 0.372f;
            float startColorGreen = 0.498f;
            float startColorBlue = 0.8f;
            float startColorAlpha = 0.5f;

            float startColorRedVariance = 0.0f;
            float startColorGreenVariance = 0.0f;
            float startColorBlueVariance = 0.0f;
            float startColorAlphaVariance = 0.0f;

            float finishColorRed = 0.0f;
            float finishColorGreen = 0.0f;
            float finishColorBlue = 0.0f;
            float finishColorAlpha = 0.0f;

            float finishColorRedVariance = 0.0f;
            float finishColorGreenVariance = 0.0f;
            float finishColorBlueVariance = 0.0f;
            float finishColorAlphaVariance = 0.0f;

            float emissionRate = 0.0f;

            std::string textureFilename;
        };
    } // namespace scene
} // namespace ouzel
