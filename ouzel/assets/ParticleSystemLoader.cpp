// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "ParticleSystemLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "scene/ParticleSystemData.hpp"
#include "utils/JSON.hpp"

namespace ouzel
{
    namespace assets
    {
        ParticleSystemLoader::ParticleSystemLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool ParticleSystemLoader::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            scene::ParticleSystemData particleSystemData;

            json::Data document(data);

            if (!document.hasMember("textureFileName") ||
                !document.hasMember("configName"))
                return false;

            particleSystemData.name = document["configName"].as<std::string>();

            if (document.hasMember("blendFuncSource")) particleSystemData.blendFuncSource = document["blendFuncSource"].as<uint32_t>();
            if (document.hasMember("blendFuncDestination")) particleSystemData.blendFuncDestination = document["blendFuncDestination"].as<uint32_t>();

            if (document.hasMember("emitterType"))
            {
                switch (document["emitterType"].as<uint32_t>())
                {
                    case 0: particleSystemData.emitterType = scene::ParticleSystemData::EmitterType::GRAVITY; break;
                    case 1: particleSystemData.emitterType = scene::ParticleSystemData::EmitterType::RADIUS; break;
                    default: return false;
                }
            }

            if (document.hasMember("maxParticles")) particleSystemData.maxParticles = document["maxParticles"].as<uint32_t>();

            if (document.hasMember("duration")) particleSystemData.duration = document["duration"].as<float>();
            if (document.hasMember("particleLifespan")) particleSystemData.particleLifespan = document["particleLifespan"].as<float>();
            if (document.hasMember("particleLifespanVariance")) particleSystemData.particleLifespanVariance = document["particleLifespanVariance"].as<float>();

            if (document.hasMember("speed")) particleSystemData.speed = document["speed"].as<float>();
            if (document.hasMember("speedVariance")) particleSystemData.speedVariance = document["speedVariance"].as<float>();

            if (document.hasMember("absolutePosition")) particleSystemData.absolutePosition = document["absolutePosition"].as<bool>();

            if (document.hasMember("yCoordFlipped")) particleSystemData.yCoordFlipped = (document["yCoordFlipped"].as<uint32_t>() == 1);

            if (document.hasMember("sourcePositionx")) particleSystemData.sourcePosition.v[0] = document["sourcePositionx"].as<float>();
            if (document.hasMember("sourcePositiony")) particleSystemData.sourcePosition.v[1] = document["sourcePositiony"].as<float>();
            if (document.hasMember("sourcePositionVariancex")) particleSystemData.sourcePositionVariance.v[0] = document["sourcePositionVariancex"].as<float>();
            if (document.hasMember("sourcePositionVariancey")) particleSystemData.sourcePositionVariance.v[1] = document["sourcePositionVariancey"].as<float>();

            if (document.hasMember("startParticleSize")) particleSystemData.startParticleSize = document["startParticleSize"].as<float>();
            if (document.hasMember("startParticleSizeVariance")) particleSystemData.startParticleSizeVariance = document["startParticleSizeVariance"].as<float>();
            if (document.hasMember("finishParticleSize")) particleSystemData.finishParticleSize = document["finishParticleSize"].as<float>();
            if (document.hasMember("finishParticleSizeVariance")) particleSystemData.finishParticleSizeVariance = document["finishParticleSizeVariance"].as<float>();
            if (document.hasMember("angle")) particleSystemData.angle = document["angle"].as<float>();
            if (document.hasMember("angleVariance")) particleSystemData.angleVariance = document["angleVariance"].as<float>();
            if (document.hasMember("rotationStart")) particleSystemData.startRotation = document["rotationStart"].as<float>();
            if (document.hasMember("rotationStartVariance")) particleSystemData.startRotationVariance = document["rotationStartVariance"].as<float>();
            if (document.hasMember("rotationEnd")) particleSystemData.finishRotation = document["rotationEnd"].as<float>();
            if (document.hasMember("rotationEndVariance")) particleSystemData.finishRotationVariance = document["rotationEndVariance"].as<float>();
            if (document.hasMember("rotatePerSecond")) particleSystemData.rotatePerSecond = document["rotatePerSecond"].as<float>();
            if (document.hasMember("rotatePerSecondVariance")) particleSystemData.rotatePerSecondVariance = document["rotatePerSecondVariance"].as<float>();
            if (document.hasMember("minRadius")) particleSystemData.minRadius = document["minRadius"].as<float>();
            if (document.hasMember("minRadiusVariance")) particleSystemData.minRadiusVariance = document["minRadiusVariance"].as<float>();
            if (document.hasMember("maxRadius")) particleSystemData.maxRadius = document["maxRadius"].as<float>();
            if (document.hasMember("maxRadiusVariance")) particleSystemData.maxRadiusVariance = document["maxRadiusVariance"].as<float>();

            if (document.hasMember("radialAcceleration")) particleSystemData.radialAcceleration = document["radialAcceleration"].as<float>();
            if (document.hasMember("radialAccelVariance")) particleSystemData.radialAccelVariance = document["radialAccelVariance"].as<float>();
            if (document.hasMember("tangentialAcceleration")) particleSystemData.tangentialAcceleration = document["tangentialAcceleration"].as<float>();
            if (document.hasMember("tangentialAccelVariance")) particleSystemData.tangentialAccelVariance = document["tangentialAccelVariance"].as<float>();

            if (document.hasMember("rotationIsDir")) particleSystemData.rotationIsDir = document["rotationIsDir"].as<bool>();

            if (document.hasMember("gravityx")) particleSystemData.gravity.v[0] = document["gravityx"].as<float>();
            if (document.hasMember("gravityy")) particleSystemData.gravity.v[1] = document["gravityy"].as<float>();

            if (document.hasMember("startColorRed")) particleSystemData.startColorRed = document["startColorRed"].as<float>();
            if (document.hasMember("startColorGreen")) particleSystemData.startColorGreen = document["startColorGreen"].as<float>();
            if (document.hasMember("startColorBlue")) particleSystemData.startColorBlue = document["startColorBlue"].as<float>();
            if (document.hasMember("startColorAlpha")) particleSystemData.startColorAlpha = document["startColorAlpha"].as<float>();

            if (document.hasMember("startColorVarianceRed")) particleSystemData.startColorRedVariance = document["startColorVarianceRed"].as<float>();
            if (document.hasMember("startColorVarianceGreen")) particleSystemData.startColorGreenVariance = document["startColorVarianceGreen"].as<float>();
            if (document.hasMember("startColorVarianceBlue")) particleSystemData.startColorBlueVariance = document["startColorVarianceBlue"].as<float>();
            if (document.hasMember("startColorVarianceAlpha")) particleSystemData.startColorAlphaVariance = document["startColorVarianceAlpha"].as<float>();

            if (document.hasMember("finishColorRed")) particleSystemData.finishColorRed = document["finishColorRed"].as<float>();
            if (document.hasMember("finishColorGreen")) particleSystemData.finishColorGreen = document["finishColorGreen"].as<float>();
            if (document.hasMember("finishColorBlue")) particleSystemData.finishColorBlue = document["finishColorBlue"].as<float>();
            if (document.hasMember("finishColorAlpha")) particleSystemData.finishColorAlpha = document["finishColorAlpha"].as<float>();

            if (document.hasMember("finishColorVarianceRed")) particleSystemData.finishColorRedVariance = document["finishColorVarianceRed"].as<float>();
            if (document.hasMember("finishColorVarianceGreen")) particleSystemData.finishColorGreenVariance = document["finishColorVarianceGreen"].as<float>();
            if (document.hasMember("finishColorVarianceBlue")) particleSystemData.finishColorBlueVariance = document["finishColorVarianceBlue"].as<float>();
            if (document.hasMember("finishColorVarianceAlpha")) particleSystemData.finishColorAlphaVariance = document["finishColorVarianceAlpha"].as<float>();

            if (document.hasMember("textureFileName"))
            {
                std::string textureFileName = document["textureFileName"].as<std::string>();
                particleSystemData.texture = cache.getTexture(textureFileName);

                if (!particleSystemData.texture)
                {
                    bundle.loadAsset(Loader::IMAGE, textureFileName, mipmaps);
                    particleSystemData.texture = cache.getTexture(textureFileName);
                }
            }

            particleSystemData.emissionRate = static_cast<float>(particleSystemData.maxParticles) / particleSystemData.particleLifespan;

            bundle.setParticleSystemData(filename, std::move(particleSystemData));

            return true;
        }
    } // namespace assets
} // namespace ouzel
