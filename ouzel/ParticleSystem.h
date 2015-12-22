// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "node.h"
#include "Vector2.h"

namespace ouzel
{
    class Scene;
    
    class ParticleSystem: public Node
    {
    public:
        ParticleSystem(Scene* scene);
        virtual ~ParticleSystem();
        
        virtual bool initFromFile(const std::string& filename);
        
    protected:
        uint32_t _blendFuncSource = 1;
        uint32_t _blendFuncDestination = 771;
        
        uint32_t _emitterType = 0;
        uint32_t _maxParticles = 77;
        float _duration = -1;
        float _particleLifespan = 1.0f;
        float _particleLifespanVariance = 0.0f;
        
        float _speed = 225.0f;
        float _speedVariance = 30.0f;
        
        bool _absolutePosition = false;
        bool _yCoordFlipped = true;
        Vector2 _sourcePosition = Vector2(160.0f, 240.0f);
        Vector2 _sourcePositionVariance = Vector2(7.0f, 7.0f);
        
        float _startParticleSize = 64.0f;
        float _startParticleSizeVariance = 5.0f;
        
        float _finishParticleSize = 0.0f;
        float _finishParticleSizeVariance = 0.0f;
        
        float _angle = 90.0f;
        float _angleVariance = 10.0f;
        
        float _rotationStart = 0.0f;
        float _rotationStartVariance = 0.0f;
        
        float _rotationEnd = 0.0f;
        float _rotationEndVariance = 0.0f;
        
        float _rotatePerSecond = 360.0f;
        float _rotatePerSecondVariance = 0.0f;
        
        float _minRadius = 300.0f;
        float _minRadiusVariance = 0.0f;
        
        float _maxRadius = 0.0f;
        float _maxRadiusVariance = 0.0f;
        
        float _radialAcceleration = 0.0f;
        float _radialAccelVariance = 0.0f;
        
        float _tangentialAcceleration = 0.0f;
        float _tangentialAccelVariance = 0.0f;
        
        Vector2 _gravity = Vector2(0.0f, 0.0f);
        
        float _startColorRed = 0.372f;
        float _startColorGreen = 0.498f;
        float _startColorBlue = 0.8f;
        float _startColorAlpha = 0.5f;
        
        float _startColorVarianceRed = 0.0f;
        float _startColorVarianceGreen = 0.0f;
        float _startColorVarianceBlue = 0.0f;
        float _startColorVarianceAlpha = 0.0f;
        
        float _finishColorRed = 0.0f;
        float _finishColorGreen = 0.0f;
        float _finishColorBlue = 0.0f;
        float _finishColorAlpha = 0.0f;
        
        float _finishColorVarianceRed = 0.0f;
        float _finishColorVarianceGreen = 0.0f;
        float _finishColorVarianceBlue = 0.0f;
        float _finishColorVarianceAlpha = 0.0f;

        std::string _textureFilename;
        
        
        
    };
}
