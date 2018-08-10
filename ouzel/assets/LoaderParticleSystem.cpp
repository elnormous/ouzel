// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "LoaderParticleSystem.hpp"
#include "Cache.hpp"
#include "files/FileSystem.hpp"
#include "scene/ParticleSystemData.hpp"
#include "utils/JSON.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderParticleSystem::LoaderParticleSystem():
            Loader(TYPE)
        {
        }

        bool LoaderParticleSystem::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            scene::ParticleSystemData particleSystemData;

            json::Data document(data);

            if (!document.hasMember("textureFileName") ||
                !document.hasMember("configName"))
                return false;

            particleSystemData.name = document["configName"].asString();

            if (document.hasMember("blendFuncSource")) particleSystemData.blendFuncSource = document["blendFuncSource"].asUInt32();
            if (document.hasMember("blendFuncDestination")) particleSystemData.blendFuncDestination = document["blendFuncDestination"].asUInt32();

            if (document.hasMember("emitterType"))
            {
                switch (document["emitterType"].asUInt32())
                {
                    case 0: particleSystemData.emitterType = scene::ParticleSystemData::EmitterType::GRAVITY; break;
                    case 1: particleSystemData.emitterType = scene::ParticleSystemData::EmitterType::RADIUS; break;
                    default: return false;
                }
            }

            if (document.hasMember("maxParticles")) particleSystemData.maxParticles = document["maxParticles"].asUInt32();

            if (document.hasMember("duration")) particleSystemData.duration = document["duration"].asFloat();
            if (document.hasMember("particleLifespan")) particleSystemData.particleLifespan = document["particleLifespan"].asFloat();
            if (document.hasMember("particleLifespanVariance")) particleSystemData.particleLifespanVariance = document["particleLifespanVariance"].asFloat();

            if (document.hasMember("speed")) particleSystemData.speed = document["speed"].asFloat();
            if (document.hasMember("speedVariance")) particleSystemData.speedVariance = document["speedVariance"].asFloat();

            if (document.hasMember("absolutePosition")) particleSystemData.absolutePosition = document["absolutePosition"].asBool();

            if (document.hasMember("yCoordFlipped")) particleSystemData.yCoordFlipped = (document["yCoordFlipped"].asUInt32() == 1);

            if (document.hasMember("sourcePositionx")) particleSystemData.sourcePosition.x = document["sourcePositionx"].asFloat();
            if (document.hasMember("sourcePositiony")) particleSystemData.sourcePosition.y = document["sourcePositiony"].asFloat();
            if (document.hasMember("sourcePositionVariancex")) particleSystemData.sourcePositionVariance.x = document["sourcePositionVariancex"].asFloat();
            if (document.hasMember("sourcePositionVariancey")) particleSystemData.sourcePositionVariance.y = document["sourcePositionVariancey"].asFloat();

            if (document.hasMember("startParticleSize")) particleSystemData.startParticleSize = document["startParticleSize"].asFloat();
            if (document.hasMember("startParticleSizeVariance")) particleSystemData.startParticleSizeVariance = document["startParticleSizeVariance"].asFloat();
            if (document.hasMember("finishParticleSize")) particleSystemData.finishParticleSize = document["finishParticleSize"].asFloat();
            if (document.hasMember("finishParticleSizeVariance")) particleSystemData.finishParticleSizeVariance = document["finishParticleSizeVariance"].asFloat();
            if (document.hasMember("angle")) particleSystemData.angle = document["angle"].asFloat();
            if (document.hasMember("angleVariance")) particleSystemData.angleVariance = document["angleVariance"].asFloat();
            if (document.hasMember("rotationStart")) particleSystemData.startRotation = document["rotationStart"].asFloat();
            if (document.hasMember("rotationStartVariance")) particleSystemData.startRotationVariance = document["rotationStartVariance"].asFloat();
            if (document.hasMember("rotationEnd")) particleSystemData.finishRotation = document["rotationEnd"].asFloat();
            if (document.hasMember("rotationEndVariance")) particleSystemData.finishRotationVariance = document["rotationEndVariance"].asFloat();
            if (document.hasMember("rotatePerSecond")) particleSystemData.rotatePerSecond = document["rotatePerSecond"].asFloat();
            if (document.hasMember("rotatePerSecondVariance")) particleSystemData.rotatePerSecondVariance = document["rotatePerSecondVariance"].asFloat();
            if (document.hasMember("minRadius")) particleSystemData.minRadius = document["minRadius"].asFloat();
            if (document.hasMember("minRadiusVariance")) particleSystemData.minRadiusVariance = document["minRadiusVariance"].asFloat();
            if (document.hasMember("maxRadius")) particleSystemData.maxRadius = document["maxRadius"].asFloat();
            if (document.hasMember("maxRadiusVariance")) particleSystemData.maxRadiusVariance = document["maxRadiusVariance"].asFloat();

            if (document.hasMember("radialAcceleration")) particleSystemData.radialAcceleration = document["radialAcceleration"].asFloat();
            if (document.hasMember("radialAccelVariance")) particleSystemData.radialAccelVariance = document["radialAccelVariance"].asFloat();
            if (document.hasMember("tangentialAcceleration")) particleSystemData.tangentialAcceleration = document["tangentialAcceleration"].asFloat();
            if (document.hasMember("tangentialAccelVariance")) particleSystemData.tangentialAccelVariance = document["tangentialAccelVariance"].asFloat();

            if (document.hasMember("rotationIsDir")) particleSystemData.rotationIsDir = document["rotationIsDir"].asBool();

            if (document.hasMember("gravityx")) particleSystemData.gravity.x = document["gravityx"].asFloat();
            if (document.hasMember("gravityy")) particleSystemData.gravity.y = document["gravityy"].asFloat();

            if (document.hasMember("startColorRed")) particleSystemData.startColorRed = document["startColorRed"].asFloat();
            if (document.hasMember("startColorGreen")) particleSystemData.startColorGreen = document["startColorGreen"].asFloat();
            if (document.hasMember("startColorBlue")) particleSystemData.startColorBlue = document["startColorBlue"].asFloat();
            if (document.hasMember("startColorAlpha")) particleSystemData.startColorAlpha = document["startColorAlpha"].asFloat();

            if (document.hasMember("startColorVarianceRed")) particleSystemData.startColorRedVariance = document["startColorVarianceRed"].asFloat();
            if (document.hasMember("startColorVarianceGreen")) particleSystemData.startColorGreenVariance = document["startColorVarianceGreen"].asFloat();
            if (document.hasMember("startColorVarianceBlue")) particleSystemData.startColorBlueVariance = document["startColorVarianceBlue"].asFloat();
            if (document.hasMember("startColorVarianceAlpha")) particleSystemData.startColorAlphaVariance = document["startColorVarianceAlpha"].asFloat();

            if (document.hasMember("finishColorRed")) particleSystemData.finishColorRed = document["finishColorRed"].asFloat();
            if (document.hasMember("finishColorGreen")) particleSystemData.finishColorGreen = document["finishColorGreen"].asFloat();
            if (document.hasMember("finishColorBlue")) particleSystemData.finishColorBlue = document["finishColorBlue"].asFloat();
            if (document.hasMember("finishColorAlpha")) particleSystemData.finishColorAlpha = document["finishColorAlpha"].asFloat();

            if (document.hasMember("finishColorVarianceRed")) particleSystemData.finishColorRedVariance = document["finishColorVarianceRed"].asFloat();
            if (document.hasMember("finishColorVarianceGreen")) particleSystemData.finishColorGreenVariance = document["finishColorVarianceGreen"].asFloat();
            if (document.hasMember("finishColorVarianceBlue")) particleSystemData.finishColorBlueVariance = document["finishColorVarianceBlue"].asFloat();
            if (document.hasMember("finishColorVarianceAlpha")) particleSystemData.finishColorAlphaVariance = document["finishColorVarianceAlpha"].asFloat();

            if (document.hasMember("textureFileName")) particleSystemData.texture = cache->getTexture(document["textureFileName"].asString());

            particleSystemData.emissionRate = static_cast<float>(particleSystemData.maxParticles) / particleSystemData.particleLifespan;

            cache->setParticleSystemData(filename, particleSystemData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
