// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "json.hpp"
#include "ParticleSystemData.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace scene
    {
        bool ParticleSystemData::init(const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data;
            if (!sharedEngine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data, mipmaps);
        }

        bool ParticleSystemData::init(const std::vector<uint8_t>& data, bool mipmaps)
        {
            nlohmann::json document = nlohmann::json::parse(data);

            if (document.find("textureFileName") == document.end() ||
                document.find("configName") == document.end())
            {
                return false;
            }

            name = document["configName"];

            if (document.find("blendFuncSource") != document.end()) blendFuncSource = document["blendFuncSource"].get<unsigned int>();
            if (document.find("blendFuncDestination") != document.end()) blendFuncDestination = document["blendFuncDestination"].get<unsigned int>();

            if (document.find("emitterType") != document.end())
            {
                switch (document["emitterType"].get<unsigned int>())
                {
                    case 0: emitterType = ParticleSystemData::EmitterType::GRAVITY; break;
                    case 1: emitterType = ParticleSystemData::EmitterType::RADIUS; break;
                }
            }

            if (document.find("maxParticles") != document.end()) maxParticles = document["maxParticles"].get<uint32_t>();

            if (document.find("duration") != document.end()) duration = document["duration"].get<float>();
            if (document.find("particleLifespan") != document.end()) particleLifespan = document["particleLifespan"].get<float>();
            if (document.find("particleLifespanVariance") != document.end()) particleLifespanVariance = document["particleLifespanVariance"].get<float>();

            if (document.find("speed") != document.end()) speed = document["speed"].get<float>();
            if (document.find("speedVariance") != document.end()) speedVariance = document["speedVariance"].get<float>();

            if (document.find("absolutePosition") != document.end()) absolutePosition = document["absolutePosition"].get<bool>();

            if (document.find("yCoordFlipped") != document.end()) yCoordFlipped = (document["yCoordFlipped"].get<uint32_t>() == 1);

            if (document.find("sourcePositionx") != document.end()) sourcePosition.x = document["sourcePositionx"].get<float>();
            if (document.find("sourcePositiony") != document.end()) sourcePosition.y = document["sourcePositiony"].get<float>();
            if (document.find("sourcePositionVariancex") != document.end()) sourcePositionVariance.x = document["sourcePositionVariancex"].get<float>();
            if (document.find("sourcePositionVariancey") != document.end()) sourcePositionVariance.y = document["sourcePositionVariancey"].get<float>();

            if (document.find("startParticleSize") != document.end()) startParticleSize = document["startParticleSize"].get<float>();
            if (document.find("startParticleSizeVariance") != document.end()) startParticleSizeVariance = document["startParticleSizeVariance"].get<float>();
            if (document.find("finishParticleSize") != document.end()) finishParticleSize = document["finishParticleSize"].get<float>();
            if (document.find("finishParticleSizeVariance") != document.end()) finishParticleSizeVariance = document["finishParticleSizeVariance"].get<float>();
            if (document.find("angle") != document.end()) angle = document["angle"].get<float>();
            if (document.find("angleVariance") != document.end()) angleVariance = document["angleVariance"].get<float>();
            if (document.find("rotationStart") != document.end()) startRotation = document["rotationStart"].get<float>();
            if (document.find("rotationStartVariance") != document.end()) startRotationVariance = document["rotationStartVariance"].get<float>();
            if (document.find("rotationEnd") != document.end()) finishRotation = document["rotationEnd"].get<float>();
            if (document.find("rotationEndVariance") != document.end()) finishRotationVariance = document["rotationEndVariance"].get<float>();
            if (document.find("rotatePerSecond") != document.end()) rotatePerSecond = document["rotatePerSecond"].get<float>();
            if (document.find("rotatePerSecondVariance") != document.end()) rotatePerSecondVariance = document["rotatePerSecondVariance"].get<float>();
            if (document.find("minRadius") != document.end()) minRadius = document["minRadius"].get<float>();
            if (document.find("minRadiusVariance") != document.end()) minRadiusVariance = document["minRadiusVariance"].get<float>();
            if (document.find("maxRadius") != document.end()) maxRadius = document["maxRadius"].get<float>();
            if (document.find("maxRadiusVariance") != document.end()) maxRadiusVariance = document["maxRadiusVariance"].get<float>();

            if (document.find("radialAcceleration") != document.end()) radialAcceleration = document["radialAcceleration"].get<float>();
            if (document.find("radialAccelVariance") != document.end()) radialAccelVariance = document["radialAccelVariance"].get<float>();
            if (document.find("tangentialAcceleration") != document.end()) tangentialAcceleration = document["tangentialAcceleration"].get<float>();
            if (document.find("tangentialAccelVariance") != document.end()) tangentialAccelVariance = document["tangentialAccelVariance"].get<float>();

            if (document.find("rotationIsDir") != document.end()) rotationIsDir = document["rotationIsDir"].get<bool>();

            if (document.find("gravityx") != document.end()) gravity.x = document["gravityx"].get<float>();
            if (document.find("gravityy") != document.end()) gravity.y = document["gravityy"].get<float>();

            if (document.find("startColorRed") != document.end()) startColorRed = document["startColorRed"].get<float>();
            if (document.find("startColorGreen") != document.end()) startColorGreen = document["startColorGreen"].get<float>();
            if (document.find("startColorBlue") != document.end()) startColorBlue = document["startColorBlue"].get<float>();
            if (document.find("startColorAlpha") != document.end()) startColorAlpha = document["startColorAlpha"].get<float>();

            if (document.find("startColorVarianceRed") != document.end()) startColorRedVariance = document["startColorVarianceRed"].get<float>();
            if (document.find("startColorVarianceGreen") != document.end()) startColorGreenVariance = document["startColorVarianceGreen"].get<float>();
            if (document.find("startColorVarianceBlue") != document.end()) startColorBlueVariance = document["startColorVarianceBlue"].get<float>();
            if (document.find("startColorVarianceAlpha") != document.end()) startColorAlphaVariance = document["startColorVarianceAlpha"].get<float>();

            if (document.find("finishColorRed") != document.end()) finishColorRed = document["finishColorRed"].get<float>();
            if (document.find("finishColorGreen") != document.end()) finishColorGreen = document["finishColorGreen"].get<float>();
            if (document.find("finishColorBlue") != document.end()) finishColorBlue = document["finishColorBlue"].get<float>();
            if (document.find("finishColorAlpha") != document.end()) finishColorAlpha = document["finishColorAlpha"].get<float>();

            if (document.find("finishColorVarianceRed") != document.end()) finishColorRedVariance = document["finishColorVarianceRed"].get<float>();
            if (document.find("finishColorVarianceGreen") != document.end()) finishColorGreenVariance = document["finishColorVarianceGreen"].get<float>();
            if (document.find("finishColorVarianceBlue") != document.end()) finishColorBlueVariance = document["finishColorVarianceBlue"].get<float>();
            if (document.find("finishColorVarianceAlpha") != document.end()) finishColorAlphaVariance = document["finishColorVarianceAlpha"].get<float>();

            if (document.find("textureFileName") != document.end()) texture = sharedEngine->getCache()->getTexture(document["textureFileName"].get<std::string>(), mipmaps);

            emissionRate = static_cast<float>(maxParticles) / particleLifespan;

            return true;
        }
    } // namespace scene
} // namespace ouzel
