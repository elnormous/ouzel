// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "json.hpp"
#include "ParticleDefinition.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace scene
    {
        bool ParticleDefinition::load(const std::string& filename, bool mipmaps, ParticleDefinition& particleDefinition)
        {
            std::vector<uint8_t> data;
            if (!sharedEngine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            nlohmann::json document = nlohmann::json::parse(data);

            if (document.find("blendFuncSource") != document.end()) particleDefinition.blendFuncSource = document["blendFuncSource"].get<unsigned int>();
            if (document.find("blendFuncDestination") != document.end()) particleDefinition.blendFuncDestination = document["blendFuncDestination"].get<unsigned int>();

            if (document.find("emitterType") != document.end())
            {
                uint32_t emitterType = document["emitterType"].get<unsigned int>();

                switch (emitterType)
                {
                    case 0: particleDefinition.emitterType = ParticleDefinition::EmitterType::GRAVITY; break;
                    case 1: particleDefinition.emitterType = ParticleDefinition::EmitterType::RADIUS; break;
                }
            }

            if (document.find("maxParticles") != document.end()) particleDefinition.maxParticles = document["maxParticles"].get<uint32_t>();

            if (document.find("duration") != document.end()) particleDefinition.duration = document["duration"].get<float>();
            if (document.find("particleLifespan") != document.end()) particleDefinition.particleLifespan = document["particleLifespan"].get<float>();
            if (document.find("particleLifespanVariance") != document.end()) particleDefinition.particleLifespanVariance = document["particleLifespanVariance"].get<float>();

            if (document.find("speed") != document.end()) particleDefinition.speed = document["speed"].get<float>();
            if (document.find("speedVariance") != document.end()) particleDefinition.speedVariance = document["speedVariance"].get<float>();

            if (document.find("absolutePosition") != document.end()) particleDefinition.absolutePosition = document["absolutePosition"].get<bool>();

            if (document.find("yCoordFlipped") != document.end()) particleDefinition.yCoordFlipped = (document["yCoordFlipped"].get<uint32_t>() == 1);

            if (document.find("sourcePositionx") != document.end()) particleDefinition.sourcePosition.x = document["sourcePositionx"].get<float>();
            if (document.find("sourcePositiony") != document.end()) particleDefinition.sourcePosition.y = document["sourcePositiony"].get<float>();
            if (document.find("sourcePositionVariancex") != document.end()) particleDefinition.sourcePositionVariance.x = document["sourcePositionVariancex"].get<float>();
            if (document.find("sourcePositionVariancey") != document.end()) particleDefinition.sourcePositionVariance.y = document["sourcePositionVariancey"].get<float>();

            if (document.find("startParticleSize") != document.end()) particleDefinition.startParticleSize = document["startParticleSize"].get<float>();
            if (document.find("startParticleSizeVariance") != document.end()) particleDefinition.startParticleSizeVariance = document["startParticleSizeVariance"].get<float>();
            if (document.find("finishParticleSize") != document.end()) particleDefinition.finishParticleSize = document["finishParticleSize"].get<float>();
            if (document.find("finishParticleSizeVariance") != document.end()) particleDefinition.finishParticleSizeVariance = document["finishParticleSizeVariance"].get<float>();
            if (document.find("angle") != document.end()) particleDefinition.angle = document["angle"].get<float>();
            if (document.find("angleVariance") != document.end()) particleDefinition.angleVariance = document["angleVariance"].get<float>();
            if (document.find("rotationStart") != document.end()) particleDefinition.startRotation = document["rotationStart"].get<float>();
            if (document.find("rotationStartVariance") != document.end()) particleDefinition.startRotationVariance = document["rotationStartVariance"].get<float>();
            if (document.find("rotationEnd") != document.end()) particleDefinition.finishRotation = document["rotationEnd"].get<float>();
            if (document.find("rotationEndVariance") != document.end()) particleDefinition.finishRotationVariance = document["rotationEndVariance"].get<float>();
            if (document.find("rotatePerSecond") != document.end()) particleDefinition.rotatePerSecond = document["rotatePerSecond"].get<float>();
            if (document.find("rotatePerSecondVariance") != document.end()) particleDefinition.rotatePerSecondVariance = document["rotatePerSecondVariance"].get<float>();
            if (document.find("minRadius") != document.end()) particleDefinition.minRadius = document["minRadius"].get<float>();
            if (document.find("minRadiusVariance") != document.end()) particleDefinition.minRadiusVariance = document["minRadiusVariance"].get<float>();
            if (document.find("maxRadius") != document.end()) particleDefinition.maxRadius = document["maxRadius"].get<float>();
            if (document.find("maxRadiusVariance") != document.end()) particleDefinition.maxRadiusVariance = document["maxRadiusVariance"].get<float>();

            if (document.find("radialAcceleration") != document.end()) particleDefinition.radialAcceleration = document["radialAcceleration"].get<float>();
            if (document.find("radialAccelVariance") != document.end()) particleDefinition.radialAccelVariance = document["radialAccelVariance"].get<float>();
            if (document.find("tangentialAcceleration") != document.end()) particleDefinition.tangentialAcceleration = document["tangentialAcceleration"].get<float>();
            if (document.find("tangentialAccelVariance") != document.end()) particleDefinition.tangentialAccelVariance = document["tangentialAccelVariance"].get<float>();

            if (document.find("rotationIsDir") != document.end()) particleDefinition.rotationIsDir = document["rotationIsDir"].get<bool>();

            if (document.find("gravityx") != document.end()) particleDefinition.gravity.x = document["gravityx"].get<float>();
            if (document.find("gravityy") != document.end()) particleDefinition.gravity.y = document["gravityy"].get<float>();

            if (document.find("startColorRed") != document.end()) particleDefinition.startColorRed = document["startColorRed"].get<float>();
            if (document.find("startColorGreen") != document.end()) particleDefinition.startColorGreen = document["startColorGreen"].get<float>();
            if (document.find("startColorBlue") != document.end()) particleDefinition.startColorBlue = document["startColorBlue"].get<float>();
            if (document.find("startColorAlpha") != document.end()) particleDefinition.startColorAlpha = document["startColorAlpha"].get<float>();

            if (document.find("startColorVarianceRed") != document.end()) particleDefinition.startColorRedVariance = document["startColorVarianceRed"].get<float>();
            if (document.find("startColorVarianceGreen") != document.end()) particleDefinition.startColorGreenVariance = document["startColorVarianceGreen"].get<float>();
            if (document.find("startColorVarianceBlue") != document.end()) particleDefinition.startColorBlueVariance = document["startColorVarianceBlue"].get<float>();
            if (document.find("startColorVarianceAlpha") != document.end()) particleDefinition.startColorAlphaVariance = document["startColorVarianceAlpha"].get<float>();

            if (document.find("finishColorRed") != document.end()) particleDefinition.finishColorRed = document["finishColorRed"].get<float>();
            if (document.find("finishColorGreen") != document.end()) particleDefinition.finishColorGreen = document["finishColorGreen"].get<float>();
            if (document.find("finishColorBlue") != document.end()) particleDefinition.finishColorBlue = document["finishColorBlue"].get<float>();
            if (document.find("finishColorAlpha") != document.end()) particleDefinition.finishColorAlpha = document["finishColorAlpha"].get<float>();

            if (document.find("finishColorVarianceRed") != document.end()) particleDefinition.finishColorRedVariance = document["finishColorVarianceRed"].get<float>();
            if (document.find("finishColorVarianceGreen") != document.end()) particleDefinition.finishColorGreenVariance = document["finishColorVarianceGreen"].get<float>();
            if (document.find("finishColorVarianceBlue") != document.end()) particleDefinition.finishColorBlueVariance = document["finishColorVarianceBlue"].get<float>();
            if (document.find("finishColorVarianceAlpha") != document.end()) particleDefinition.finishColorAlphaVariance = document["finishColorVarianceAlpha"].get<float>();

            if (document.find("textureFileName") != document.end()) particleDefinition.texture = sharedEngine->getCache()->getTexture(document["textureFileName"].get<std::string>(), mipmaps);

            particleDefinition.emissionRate = static_cast<float>(particleDefinition.maxParticles) / particleDefinition.particleLifespan;

            return true;
        }
    } // namespace scene
} // namespace ouzel
