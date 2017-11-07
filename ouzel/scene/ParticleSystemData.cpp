// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ParticleSystemData.hpp"
#include "core/Engine.hpp"
#include "files/FileSystem.hpp"
#include "utils/JSON.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace scene
    {
        bool ParticleSystemData::init(const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data;
            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data, mipmaps);
        }

        bool ParticleSystemData::init(const std::vector<uint8_t>& data, bool mipmaps)
        {
            json::Data document;

            if (!document.init(data))
            {
                return false;
            }

            if (!document.hasMember("textureFileName") ||
                !document.hasMember("configName"))
            {
                return false;
            }

            name = document["configName"].asString();

            if (document.hasMember("blendFuncSource")) blendFuncSource = document["blendFuncSource"].asUInt32();
            if (document.hasMember("blendFuncDestination")) blendFuncDestination = document["blendFuncDestination"].asUInt32();

            if (document.hasMember("emitterType"))
            {
                switch (document["emitterType"].asUInt32())
                {
                    case 0: emitterType = ParticleSystemData::EmitterType::GRAVITY; break;
                    case 1: emitterType = ParticleSystemData::EmitterType::RADIUS; break;
                }
            }

            if (document.hasMember("maxParticles")) maxParticles = document["maxParticles"].asUInt32();

            if (document.hasMember("duration")) duration = document["duration"].asFloat();
            if (document.hasMember("particleLifespan")) particleLifespan = document["particleLifespan"].asFloat();
            if (document.hasMember("particleLifespanVariance")) particleLifespanVariance = document["particleLifespanVariance"].asFloat();

            if (document.hasMember("speed")) speed = document["speed"].asFloat();
            if (document.hasMember("speedVariance")) speedVariance = document["speedVariance"].asFloat();

            if (document.hasMember("absolutePosition")) absolutePosition = document["absolutePosition"].asBool();

            if (document.hasMember("yCoordFlipped")) yCoordFlipped = (document["yCoordFlipped"].asUInt32() == 1);

            if (document.hasMember("sourcePositionx")) sourcePosition.x = document["sourcePositionx"].asFloat();
            if (document.hasMember("sourcePositiony")) sourcePosition.y = document["sourcePositiony"].asFloat();
            if (document.hasMember("sourcePositionVariancex")) sourcePositionVariance.x = document["sourcePositionVariancex"].asFloat();
            if (document.hasMember("sourcePositionVariancey")) sourcePositionVariance.y = document["sourcePositionVariancey"].asFloat();

            if (document.hasMember("startParticleSize")) startParticleSize = document["startParticleSize"].asFloat();
            if (document.hasMember("startParticleSizeVariance")) startParticleSizeVariance = document["startParticleSizeVariance"].asFloat();
            if (document.hasMember("finishParticleSize")) finishParticleSize = document["finishParticleSize"].asFloat();
            if (document.hasMember("finishParticleSizeVariance")) finishParticleSizeVariance = document["finishParticleSizeVariance"].asFloat();
            if (document.hasMember("angle")) angle = document["angle"].asFloat();
            if (document.hasMember("angleVariance")) angleVariance = document["angleVariance"].asFloat();
            if (document.hasMember("rotationStart")) startRotation = document["rotationStart"].asFloat();
            if (document.hasMember("rotationStartVariance")) startRotationVariance = document["rotationStartVariance"].asFloat();
            if (document.hasMember("rotationEnd")) finishRotation = document["rotationEnd"].asFloat();
            if (document.hasMember("rotationEndVariance")) finishRotationVariance = document["rotationEndVariance"].asFloat();
            if (document.hasMember("rotatePerSecond")) rotatePerSecond = document["rotatePerSecond"].asFloat();
            if (document.hasMember("rotatePerSecondVariance")) rotatePerSecondVariance = document["rotatePerSecondVariance"].asFloat();
            if (document.hasMember("minRadius")) minRadius = document["minRadius"].asFloat();
            if (document.hasMember("minRadiusVariance")) minRadiusVariance = document["minRadiusVariance"].asFloat();
            if (document.hasMember("maxRadius")) maxRadius = document["maxRadius"].asFloat();
            if (document.hasMember("maxRadiusVariance")) maxRadiusVariance = document["maxRadiusVariance"].asFloat();

            if (document.hasMember("radialAcceleration")) radialAcceleration = document["radialAcceleration"].asFloat();
            if (document.hasMember("radialAccelVariance")) radialAccelVariance = document["radialAccelVariance"].asFloat();
            if (document.hasMember("tangentialAcceleration")) tangentialAcceleration = document["tangentialAcceleration"].asFloat();
            if (document.hasMember("tangentialAccelVariance")) tangentialAccelVariance = document["tangentialAccelVariance"].asFloat();

            if (document.hasMember("rotationIsDir")) rotationIsDir = document["rotationIsDir"].asBool();

            if (document.hasMember("gravityx")) gravity.x = document["gravityx"].asFloat();
            if (document.hasMember("gravityy")) gravity.y = document["gravityy"].asFloat();

            if (document.hasMember("startColorRed")) startColorRed = document["startColorRed"].asFloat();
            if (document.hasMember("startColorGreen")) startColorGreen = document["startColorGreen"].asFloat();
            if (document.hasMember("startColorBlue")) startColorBlue = document["startColorBlue"].asFloat();
            if (document.hasMember("startColorAlpha")) startColorAlpha = document["startColorAlpha"].asFloat();

            if (document.hasMember("startColorVarianceRed")) startColorRedVariance = document["startColorVarianceRed"].asFloat();
            if (document.hasMember("startColorVarianceGreen")) startColorGreenVariance = document["startColorVarianceGreen"].asFloat();
            if (document.hasMember("startColorVarianceBlue")) startColorBlueVariance = document["startColorVarianceBlue"].asFloat();
            if (document.hasMember("startColorVarianceAlpha")) startColorAlphaVariance = document["startColorVarianceAlpha"].asFloat();

            if (document.hasMember("finishColorRed")) finishColorRed = document["finishColorRed"].asFloat();
            if (document.hasMember("finishColorGreen")) finishColorGreen = document["finishColorGreen"].asFloat();
            if (document.hasMember("finishColorBlue")) finishColorBlue = document["finishColorBlue"].asFloat();
            if (document.hasMember("finishColorAlpha")) finishColorAlpha = document["finishColorAlpha"].asFloat();

            if (document.hasMember("finishColorVarianceRed")) finishColorRedVariance = document["finishColorVarianceRed"].asFloat();
            if (document.hasMember("finishColorVarianceGreen")) finishColorGreenVariance = document["finishColorVarianceGreen"].asFloat();
            if (document.hasMember("finishColorVarianceBlue")) finishColorBlueVariance = document["finishColorVarianceBlue"].asFloat();
            if (document.hasMember("finishColorVarianceAlpha")) finishColorAlphaVariance = document["finishColorVarianceAlpha"].asFloat();

            if (document.hasMember("textureFileName")) texture = engine->getCache()->getTexture(document["textureFileName"].asString(), mipmaps);

            emissionRate = static_cast<float>(maxParticles) / particleLifespan;

            return true;
        }
    } // namespace scene
} // namespace ouzel
