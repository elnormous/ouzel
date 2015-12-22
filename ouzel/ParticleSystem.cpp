// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ParticleSystem.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Utils.h"

namespace ouzel
{
    ParticleSystem::ParticleSystem(Scene* scene):
        Node(scene)
    {
        
    }
    
    ParticleSystem::~ParticleSystem()
    {
        
    }
    
    bool ParticleSystem::initFromFile(const std::string& filename)
    {
        FILE* fp = fopen(filename.c_str(), "r");
        
        if (fp)
        {
            rapidjson::FileReadStream is(fp, TEMP_BUFFER, sizeof(TEMP_BUFFER));
            
            rapidjson::Document document;
            document.ParseStream<0>(is);
            
            if (document.HasParseError())
            {
                fclose(fp);
                return false;
            }
            
            if (document.HasMember("blendFuncSource")) _blendFuncSource = document["blendFuncSource"].GetInt();
            if (document.HasMember("blendFuncDestination")) _blendFuncDestination = document["blendFuncDestination"].GetInt();
            if (document.HasMember("emitterType")) _emitterType = document["emitterType"].GetInt();
            if (document.HasMember("maxParticles")) _maxParticles = document["maxParticles"].GetInt();
            
            if (document.HasMember("duration")) _duration = static_cast<float>(document["duration"].GetDouble());
            if (document.HasMember("particleLifespan")) _particleLifespan = static_cast<float>(document["particleLifespan"].GetDouble());
            if (document.HasMember("particleLifespanVariance")) _duration = static_cast<float>(document["particleLifespanVariance"].GetDouble());
            
            if (document.HasMember("speed")) _speed = static_cast<float>(document["speed"].GetDouble());
            if (document.HasMember("speedVariance")) _speedVariance = static_cast<float>(document["speedVariance"].GetDouble());
            
            if (document.HasMember("absolutePosition")) _absolutePosition = document["absolutePosition"].GetBool();
            
            if (document.HasMember("yCoordFlipped")) _yCoordFlipped = (document["yCoordFlipped"].GetInt() == 1);
            
            if (document.HasMember("sourcePositionx")) _sourcePosition.x = static_cast<float>(document["sourcePositionx"].GetDouble());
            if (document.HasMember("sourcePositiony")) _sourcePosition.y = static_cast<float>(document["sourcePositiony"].GetDouble());
            if (document.HasMember("sourcePositionVariancex")) _sourcePositionVariance.x = static_cast<float>(document["sourcePositionVariancex"].GetDouble());
            if (document.HasMember("sourcePositionVariancey")) _sourcePositionVariance.y = static_cast<float>(document["sourcePositionVariancey"].GetDouble());
            
            if (document.HasMember("startParticleSize")) _startParticleSize = static_cast<float>(document["startParticleSize"].GetDouble());
            if (document.HasMember("startParticleSizeVariance")) _startParticleSizeVariance = static_cast<float>(document["startParticleSizeVariance"].GetDouble());
            if (document.HasMember("finishParticleSize")) _finishParticleSize = static_cast<float>(document["finishParticleSize"].GetDouble());
            if (document.HasMember("finishParticleSizeVariance")) _finishParticleSizeVariance = static_cast<float>(document["finishParticleSizeVariance"].GetDouble());
            if (document.HasMember("angle")) _angle = static_cast<float>(document["angle"].GetDouble());
            if (document.HasMember("angleVariance")) _angleVariance = static_cast<float>(document["angleVariance"].GetDouble());
            if (document.HasMember("rotationStart")) _rotationStart = static_cast<float>(document["rotationStart"].GetDouble());
            if (document.HasMember("rotationStartVariance")) _rotationStartVariance = static_cast<float>(document["rotationStartVariance"].GetDouble());
            if (document.HasMember("rotationEnd")) _rotationEnd = static_cast<float>(document["rotationEnd"].GetDouble());
            if (document.HasMember("rotationEndVariance")) _rotationEndVariance = static_cast<float>(document["rotationEndVariance"].GetDouble());
            if (document.HasMember("rotatePerSecond")) _rotatePerSecond = static_cast<float>(document["rotatePerSecond"].GetDouble());
            if (document.HasMember("rotatePerSecondVariance")) _rotatePerSecondVariance = static_cast<float>(document["rotatePerSecondVariance"].GetDouble());
            if (document.HasMember("minRadius")) _minRadius = static_cast<float>(document["minRadius"].GetDouble());
            if (document.HasMember("minRadiusVariance")) _minRadiusVariance = static_cast<float>(document["minRadiusVariance"].GetDouble());
            if (document.HasMember("maxRadius")) _maxRadius = static_cast<float>(document["maxRadius"].GetDouble());
            if (document.HasMember("maxRadiusVariance")) _maxRadiusVariance = static_cast<float>(document["maxRadiusVariance"].GetDouble());
            
            if (document.HasMember("radialAcceleration")) _radialAcceleration = static_cast<float>(document["radialAcceleration"].GetDouble());
            if (document.HasMember("radialAccelVariance")) _radialAccelVariance = static_cast<float>(document["radialAccelVariance"].GetDouble());
            if (document.HasMember("tangentialAcceleration")) _tangentialAcceleration = static_cast<float>(document["tangentialAcceleration"].GetDouble());
            if (document.HasMember("tangentialAccelVariance")) _tangentialAccelVariance = static_cast<float>(document["tangentialAccelVariance"].GetDouble());
            
            if (document.HasMember("gravityx")) _gravity.x = static_cast<float>(document["gravityx"].GetDouble());
            if (document.HasMember("gravityy")) _gravity.y = static_cast<float>(document["gravityy"].GetDouble());
            
            if (document.HasMember("startColorRed")) _startColorRed = static_cast<float>(document["startColorRed"].GetDouble());
            if (document.HasMember("startColorGreen")) _startColorGreen = static_cast<float>(document["startColorGreen"].GetDouble());
            if (document.HasMember("startColorBlue")) _startColorBlue = static_cast<float>(document["startColorBlue"].GetDouble());
            if (document.HasMember("startColorAlpha")) _startColorAlpha = static_cast<float>(document["startColorAlpha"].GetDouble());
            
            if (document.HasMember("startColorVarianceRed")) _startColorVarianceRed = static_cast<float>(document["startColorVarianceRed"].GetDouble());
            if (document.HasMember("startColorVarianceGreen")) _startColorVarianceGreen = static_cast<float>(document["startColorVarianceGreen"].GetDouble());
            if (document.HasMember("startColorVarianceBlue")) _startColorVarianceBlue = static_cast<float>(document["startColorVarianceBlue"].GetDouble());
            if (document.HasMember("startColorVarianceAlpha")) _startColorVarianceAlpha = static_cast<float>(document["startColorVarianceAlpha"].GetDouble());
            
            if (document.HasMember("finishColorRed")) _finishColorRed = static_cast<float>(document["finishColorRed"].GetDouble());
            if (document.HasMember("finishColorGreen")) _finishColorGreen = static_cast<float>(document["finishColorGreen"].GetDouble());
            if (document.HasMember("finishColorBlue")) _finishColorBlue = static_cast<float>(document["finishColorBlue"].GetDouble());
            if (document.HasMember("finishColorAlpha")) _finishColorAlpha = static_cast<float>(document["finishColorAlpha"].GetDouble());
            
            if (document.HasMember("finishColorVarianceRed")) _finishColorVarianceRed = static_cast<float>(document["finishColorVarianceRed"].GetDouble());
            if (document.HasMember("finishColorVarianceGreen")) _finishColorVarianceGreen = static_cast<float>(document["finishColorVarianceGreen"].GetDouble());
            if (document.HasMember("finishColorVarianceBlue")) _finishColorVarianceBlue = static_cast<float>(document["finishColorVarianceBlue"].GetDouble());
            if (document.HasMember("finishColorVarianceAlpha")) _finishColorVarianceAlpha = static_cast<float>(document["finishColorVarianceAlpha"].GetDouble());
            
            if (document.HasMember("textureFilename")) _textureFilename = document["textureFilename"].GetString();
            
            fclose(fp);
        }
        else
        {
            return false;
        }
        
        return true;
    }
}
