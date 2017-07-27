// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <json.hpp>
#include "ParticleDefinition.h"
#include "core/Engine.h"
#include "files/FileSystem.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace scene
    {
        ParticleDefinition ParticleDefinition::loadParticleDefinition(const std::string& filename)
        {
            ParticleDefinition result;

            std::vector<uint8_t> data;
            if (!sharedEngine->getFileSystem()->readFile(filename, data))
            {
                return result;
            }

            nlohmann::json document = nlohmann::json::parse(data);

            if (document.find("blendFuncSource") != document.end()) result.blendFuncSource = document["blendFuncSource"].get<unsigned int>();
            if (document.find("blendFuncDestination") != document.end()) result.blendFuncDestination = document["blendFuncDestination"].get<unsigned int>();

            if (document.find("emitterType") != document.end())
            {
                uint32_t emitterType = document["emitterType"].get<unsigned int>();

                switch (emitterType)
                {
                    case 0: result.emitterType = ParticleDefinition::EmitterType::GRAVITY; break;
                    case 1: result.emitterType = ParticleDefinition::EmitterType::RADIUS; break;
                }
            }

            if (document.find("maxParticles") != document.end()) result.maxParticles = document["maxParticles"].get<uint32_t>();

            if (document.find("duration") != document.end()) result.duration = document["duration"].get<float>();
            if (document.find("particleLifespan") != document.end()) result.particleLifespan = document["particleLifespan"].get<float>();
            if (document.find("particleLifespanVariance") != document.end()) result.particleLifespanVariance = document["particleLifespanVariance"].get<float>();

            if (document.find("speed") != document.end()) result.speed = document["speed"].get<float>();
            if (document.find("speedVariance") != document.end()) result.speedVariance = document["speedVariance"].get<float>();

            if (document.find("absolutePosition") != document.end()) result.absolutePosition = document["absolutePosition"].get<bool>();

            if (document.find("yCoordFlipped") != document.end()) result.yCoordFlipped = (document["yCoordFlipped"].get<uint32_t>() == 1);

            if (document.find("sourcePositionx") != document.end()) result.sourcePosition.v[0] = document["sourcePositionx"].get<float>();
            if (document.find("sourcePositiony") != document.end()) result.sourcePosition.v[1] = document["sourcePositiony"].get<float>();
            if (document.find("sourcePositionVariancex") != document.end()) result.sourcePositionVariance.v[0] = document["sourcePositionVariancex"].get<float>();
            if (document.find("sourcePositionVariancey") != document.end()) result.sourcePositionVariance.v[1] = document["sourcePositionVariancey"].get<float>();

            if (document.find("startParticleSize") != document.end()) result.startParticleSize = document["startParticleSize"].get<float>();
            if (document.find("startParticleSizeVariance") != document.end()) result.startParticleSizeVariance = document["startParticleSizeVariance"].get<float>();
            if (document.find("finishParticleSize") != document.end()) result.finishParticleSize = document["finishParticleSize"].get<float>();
            if (document.find("finishParticleSizeVariance") != document.end()) result.finishParticleSizeVariance = document["finishParticleSizeVariance"].get<float>();
            if (document.find("angle") != document.end()) result.angle = document["angle"].get<float>();
            if (document.find("angleVariance") != document.end()) result.angleVariance = document["angleVariance"].get<float>();
            if (document.find("rotationStart") != document.end()) result.startRotation = document["rotationStart"].get<float>();
            if (document.find("rotationStartVariance") != document.end()) result.startRotationVariance = document["rotationStartVariance"].get<float>();
            if (document.find("rotationEnd") != document.end()) result.finishRotation = document["rotationEnd"].get<float>();
            if (document.find("rotationEndVariance") != document.end()) result.finishRotationVariance = document["rotationEndVariance"].get<float>();
            if (document.find("rotatePerSecond") != document.end()) result.rotatePerSecond = document["rotatePerSecond"].get<float>();
            if (document.find("rotatePerSecondVariance") != document.end()) result.rotatePerSecondVariance = document["rotatePerSecondVariance"].get<float>();
            if (document.find("minRadius") != document.end()) result.minRadius = document["minRadius"].get<float>();
            if (document.find("minRadiusVariance") != document.end()) result.minRadiusVariance = document["minRadiusVariance"].get<float>();
            if (document.find("maxRadius") != document.end()) result.maxRadius = document["maxRadius"].get<float>();
            if (document.find("maxRadiusVariance") != document.end()) result.maxRadiusVariance = document["maxRadiusVariance"].get<float>();

            if (document.find("radialAcceleration") != document.end()) result.radialAcceleration = document["radialAcceleration"].get<float>();
            if (document.find("radialAccelVariance") != document.end()) result.radialAccelVariance = document["radialAccelVariance"].get<float>();
            if (document.find("tangentialAcceleration") != document.end()) result.tangentialAcceleration = document["tangentialAcceleration"].get<float>();
            if (document.find("tangentialAccelVariance") != document.end()) result.tangentialAccelVariance = document["tangentialAccelVariance"].get<float>();

            if (document.find("rotationIsDir") != document.end()) result.rotationIsDir = document["rotationIsDir"].get<bool>();

            if (document.find("gravityx") != document.end()) result.gravity.v[0] = document["gravityx"].get<float>();
            if (document.find("gravityy") != document.end()) result.gravity.v[1] = document["gravityy"].get<float>();

            if (document.find("startColorRed") != document.end()) result.startColorRed = document["startColorRed"].get<float>();
            if (document.find("startColorGreen") != document.end()) result.startColorGreen = document["startColorGreen"].get<float>();
            if (document.find("startColorBlue") != document.end()) result.startColorBlue = document["startColorBlue"].get<float>();
            if (document.find("startColorAlpha") != document.end()) result.startColorAlpha = document["startColorAlpha"].get<float>();

            if (document.find("startColorVarianceRed") != document.end()) result.startColorRedVariance = document["startColorVarianceRed"].get<float>();
            if (document.find("startColorVarianceGreen") != document.end()) result.startColorGreenVariance = document["startColorVarianceGreen"].get<float>();
            if (document.find("startColorVarianceBlue") != document.end()) result.startColorBlueVariance = document["startColorVarianceBlue"].get<float>();
            if (document.find("startColorVarianceAlpha") != document.end()) result.startColorAlphaVariance = document["startColorVarianceAlpha"].get<float>();

            if (document.find("finishColorRed") != document.end()) result.finishColorRed = document["finishColorRed"].get<float>();
            if (document.find("finishColorGreen") != document.end()) result.finishColorGreen = document["finishColorGreen"].get<float>();
            if (document.find("finishColorBlue") != document.end()) result.finishColorBlue = document["finishColorBlue"].get<float>();
            if (document.find("finishColorAlpha") != document.end()) result.finishColorAlpha = document["finishColorAlpha"].get<float>();

            if (document.find("finishColorVarianceRed") != document.end()) result.finishColorRedVariance = document["finishColorVarianceRed"].get<float>();
            if (document.find("finishColorVarianceGreen") != document.end()) result.finishColorGreenVariance = document["finishColorVarianceGreen"].get<float>();
            if (document.find("finishColorVarianceBlue") != document.end()) result.finishColorBlueVariance = document["finishColorVarianceBlue"].get<float>();
            if (document.find("finishColorVarianceAlpha") != document.end()) result.finishColorAlphaVariance = document["finishColorVarianceAlpha"].get<float>();

            if (document.find("textureFileName") != document.end()) result.textureFilename = document["textureFileName"].get<std::string>();

            result.emissionRate = static_cast<float>(result.maxParticles) / result.particleLifespan;

            return result;
        }
    } // namespace scene
} // namespace ouzel
