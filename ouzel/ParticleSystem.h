// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "node.h"
#include "Vector2.h"
#include "Color.h"
#include "Vertex.h"

namespace ouzel
{
    class SceneManager;
    
    struct Particle
    {
        float life = 0.0f;
        
        Vector2 position;
        
        float colorRed = 0.0f;
        float colorGreen = 0.0f;
        float colorBlue = 0.0f;
        float colorAlpha = 0.0f;
        
        float deltaColorRed = 0.0f;
        float deltaColorGreen = 0.0f;
        float deltaColorBlue = 0.0f;
        float deltaColorAlpha = 0.0f;
        
        float angle = 0.0f;
        float speed = 0.0f;
        
        float size = 0.0f;
        float deltaSize = 0.0f;
        
        float rotation = 0.0f;
        float deltaRotation = 0.0f;
        
        float radialAcceleration = 0.0f;
        float tangentialAcceleration = 0.0f;
        
        Vector2 direction;
        float radius = 0.0f;
        float degreesPerSecond = 0.0f;
        float deltaRadius = 0.0f;
    };
    
    class ParticleSystem: public Node
    {
    public:
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
        
        static std::shared_ptr<ParticleSystem> createFromFile(const std::string& filename);
        
        ParticleSystem();
        virtual ~ParticleSystem();
        
        virtual void draw();
        virtual void update(float delta);
        
        virtual bool initFromFile(const std::string& filename);
        
        void resume();
        void stop();
        void reset();
        
        bool isRunning() const { return _running; }
        bool isActive() const { return _active; }
        
        void setPositionType(PositionType positionType) { _positionType = positionType; }
        PositionType getPositionType() const { return _positionType; }
        
        void setRemoveOnFinish(bool removeOnFinish) { _removeOnFinish = removeOnFinish; }
        bool getRemoveOnFinish() const { return _removeOnFinish; }
        
    protected:
        void createParticleMesh();
        void updateParticleMesh();
        
        void emitParticles(uint32_t particles);
        
        uint32_t _blendFuncSource = 1;
        uint32_t _blendFuncDestination = 771;
        
        EmitterType _emitterType = EmitterType::GRAVITY;
        uint32_t _maxParticles = 77;
        float _duration = -1.0f;
        float _particleLifespan = 1.0f;
        float _particleLifespanVariance = 0.0f;
        
        float _speed = 225.0f;
        float _speedVariance = 30.0f;
        
        bool _absolutePosition = false;
        bool _yCoordFlipped = true;
        Vector2 _sourcePosition;
        Vector2 _sourcePositionVariance;
        
        PositionType _positionType = PositionType::FREE;
        
        float _startParticleSize = 64.0f;
        float _startParticleSizeVariance = 5.0f;
        
        float _finishParticleSize = 0.0f;
        float _finishParticleSizeVariance = 0.0f;
        
        float _angle = 90.0f;
        float _angleVariance = 10.0f;
        
        float _startRotation = 0.0f;
        float _startRotationVariance = 0.0f;
        
        float _finishRotation = 0.0f;
        float _finishRotationVariance = 0.0f;
        
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
        
        bool _rotationIsDir = false;
        
        Vector2 _gravity = Vector2(0.0f, 0.0f);
        
        float _startColorRed = 0.372f;
        float _startColorGreen = 0.498f;
        float _startColorBlue = 0.8f;
        float _startColorAlpha = 0.5f;
        
        float _startColorRedVariance = 0.0f;
        float _startColorGreenVariance = 0.0f;
        float _startColorBlueVariance = 0.0f;
        float _startColorAlphaVariance = 0.0f;
        
        float _finishColorRed = 0.0f;
        float _finishColorGreen = 0.0f;
        float _finishColorBlue = 0.0f;
        float _finishColorAlpha = 0.0f;
        
        float _finishColorRedVariance = 0.0f;
        float _finishColorGreenVariance = 0.0f;
        float _finishColorBlueVariance = 0.0f;
        float _finishColorAlphaVariance = 0.0f;
        
        float _emissionRate = 0.0f;

        std::string _textureFilename;
        
        ShaderPtr _shader;
        TexturePtr _texture;
        
        std::vector<Particle> _particles;
        
        MeshBufferPtr _mesh;
        
        uint32_t _uniModelViewProj;
        
        std::vector<uint16_t> _indices;
        std::vector<VertexPCT> _vertices;
        
        uint32_t _particleCount = 0;
        
        float _emitCounter = 0.0f;
        float _elapsed = 0.0f;
        bool _active = false;
        bool _running = false;
        bool _finished = false;
        
        bool _needsMeshUpdate = false;
        
        bool _removeOnFinish = true;
        
        UpdateCallbackPtr _updateCallback;
    };
}
