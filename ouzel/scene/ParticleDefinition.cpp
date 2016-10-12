// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "ParticleDefinition.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace scene
    {
        ParticleDefinitionPtr ParticleDefinition::loadParticleDefinition(const std::string& filename)
        {
            ParticleDefinitionPtr result = std::make_shared<scene::ParticleDefinition>();

            std::vector<uint8_t> data;
            if (!sharedApplication->getFileSystem()->loadFile(filename, data))
            {
                return result;
            }

            rapidjson::MemoryStream is(reinterpret_cast<char*>(data.data()), data.size());

            rapidjson::Document document;
            document.ParseStream<0>(is);

            if (document.HasParseError())
            {
                Log(Log::Level::ERR) << "Failed to parse " << filename;
                return result;
            }

            if (document.HasMember("blendFuncSource")) result->blendFuncSource = document["blendFuncSource"].GetUint();
            if (document.HasMember("blendFuncDestination")) result->blendFuncDestination = document["blendFuncDestination"].GetUint();

            if (document.HasMember("emitterType"))
            {
                uint32_t emitterType = document["emitterType"].GetUint();

                switch (emitterType)
                {
                    case 0: result->emitterType = scene::ParticleDefinition::EmitterType::GRAVITY; break;
                    case 1: result->emitterType = scene::ParticleDefinition::EmitterType::RADIUS; break;
                }
            }

            if (document.HasMember("maxParticles")) result->maxParticles = document["maxParticles"].GetUint();

            if (document.HasMember("duration")) result->duration = document["duration"].GetFloat();
            if (document.HasMember("particleLifespan")) result->particleLifespan = document["particleLifespan"].GetFloat();
            if (document.HasMember("particleLifespanVariance")) result->particleLifespanVariance = document["particleLifespanVariance"].GetFloat();

            if (document.HasMember("speed")) result->speed = document["speed"].GetFloat();
            if (document.HasMember("speedVariance")) result->speedVariance = document["speedVariance"].GetFloat();

            if (document.HasMember("absolutePosition")) result->absolutePosition = document["absolutePosition"].GetBool();

            if (document.HasMember("yCoordFlipped")) result->yCoordFlipped = (document["yCoordFlipped"].GetUint() == 1);

            if (document.HasMember("sourcePositionx")) result->sourcePosition.x = document["sourcePositionx"].GetFloat();
            if (document.HasMember("sourcePositiony")) result->sourcePosition.y = document["sourcePositiony"].GetFloat();
            if (document.HasMember("sourcePositionVariancex")) result->sourcePositionVariance.x = document["sourcePositionVariancex"].GetFloat();
            if (document.HasMember("sourcePositionVariancey")) result->sourcePositionVariance.y = document["sourcePositionVariancey"].GetFloat();

            if (document.HasMember("startParticleSize")) result->startParticleSize = document["startParticleSize"].GetFloat();
            if (document.HasMember("startParticleSizeVariance")) result->startParticleSizeVariance = document["startParticleSizeVariance"].GetFloat();
            if (document.HasMember("finishParticleSize")) result->finishParticleSize = document["finishParticleSize"].GetFloat();
            if (document.HasMember("finishParticleSizeVariance")) result->finishParticleSizeVariance = document["finishParticleSizeVariance"].GetFloat();
            if (document.HasMember("angle")) result->angle = document["angle"].GetFloat();
            if (document.HasMember("angleVariance")) result->angleVariance = document["angleVariance"].GetFloat();
            if (document.HasMember("rotationStart")) result->startRotation = document["rotationStart"].GetFloat();
            if (document.HasMember("rotationStartVariance")) result->startRotationVariance = document["rotationStartVariance"].GetFloat();
            if (document.HasMember("rotationEnd")) result->finishRotation = document["rotationEnd"].GetFloat();
            if (document.HasMember("rotationEndVariance")) result->finishRotationVariance = document["rotationEndVariance"].GetFloat();
            if (document.HasMember("rotatePerSecond")) result->rotatePerSecond = document["rotatePerSecond"].GetFloat();
            if (document.HasMember("rotatePerSecondVariance")) result->rotatePerSecondVariance = document["rotatePerSecondVariance"].GetFloat();
            if (document.HasMember("minRadius")) result->minRadius = document["minRadius"].GetFloat();
            if (document.HasMember("minRadiusVariance")) result->minRadiusVariance = document["minRadiusVariance"].GetFloat();
            if (document.HasMember("maxRadius")) result->maxRadius = document["maxRadius"].GetFloat();
            if (document.HasMember("maxRadiusVariance")) result->maxRadiusVariance = document["maxRadiusVariance"].GetFloat();

            if (document.HasMember("radialAcceleration")) result->radialAcceleration = document["radialAcceleration"].GetFloat();
            if (document.HasMember("radialAccelVariance")) result->radialAccelVariance = document["radialAccelVariance"].GetFloat();
            if (document.HasMember("tangentialAcceleration")) result->tangentialAcceleration = document["tangentialAcceleration"].GetFloat();
            if (document.HasMember("tangentialAccelVariance")) result->tangentialAccelVariance = document["tangentialAccelVariance"].GetFloat();

            if (document.HasMember("rotationIsDir")) result->rotationIsDir = document["rotationIsDir"].GetBool();

            if (document.HasMember("gravityx")) result->gravity.x = document["gravityx"].GetFloat();
            if (document.HasMember("gravityy")) result->gravity.y = document["gravityy"].GetFloat();

            if (document.HasMember("startColorRed")) result->startColorRed = document["startColorRed"].GetFloat();
            if (document.HasMember("startColorGreen")) result->startColorGreen = document["startColorGreen"].GetFloat();
            if (document.HasMember("startColorBlue")) result->startColorBlue = document["startColorBlue"].GetFloat();
            if (document.HasMember("startColorAlpha")) result->startColorAlpha = document["startColorAlpha"].GetFloat();

            if (document.HasMember("startColorVarianceRed")) result->startColorRedVariance = document["startColorVarianceRed"].GetFloat();
            if (document.HasMember("startColorVarianceGreen")) result->startColorGreenVariance = document["startColorVarianceGreen"].GetFloat();
            if (document.HasMember("startColorVarianceBlue")) result->startColorBlueVariance = document["startColorVarianceBlue"].GetFloat();
            if (document.HasMember("startColorVarianceAlpha")) result->startColorAlphaVariance = document["startColorVarianceAlpha"].GetFloat();

            if (document.HasMember("finishColorRed")) result->finishColorRed = document["finishColorRed"].GetFloat();
            if (document.HasMember("finishColorGreen")) result->finishColorGreen = document["finishColorGreen"].GetFloat();
            if (document.HasMember("finishColorBlue")) result->finishColorBlue = document["finishColorBlue"].GetFloat();
            if (document.HasMember("finishColorAlpha")) result->finishColorAlpha = document["finishColorAlpha"].GetFloat();

            if (document.HasMember("finishColorVarianceRed")) result->finishColorRedVariance = document["finishColorVarianceRed"].GetFloat();
            if (document.HasMember("finishColorVarianceGreen")) result->finishColorGreenVariance = document["finishColorVarianceGreen"].GetFloat();
            if (document.HasMember("finishColorVarianceBlue")) result->finishColorBlueVariance = document["finishColorVarianceBlue"].GetFloat();
            if (document.HasMember("finishColorVarianceAlpha")) result->finishColorAlphaVariance = document["finishColorVarianceAlpha"].GetFloat();

            if (document.HasMember("textureFileName")) result->textureFilename = document["textureFileName"].GetString();

            result->emissionRate = static_cast<float>(result->maxParticles) / result->particleLifespan;

            return result;
        }
    } // namespace scene
} // namespace ouzel
