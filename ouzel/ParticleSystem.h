// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Node.h"
#include "ParticleDefinition.h"
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
        
        void setPositionType(ParticleDefinition::PositionType positionType) { _positionType = positionType; }
        ParticleDefinition::PositionType getPositionType() const { return _positionType; }
        
        void setRemoveOnFinish(bool removeOnFinish) { _removeOnFinish = removeOnFinish; }
        bool getRemoveOnFinish() const { return _removeOnFinish; }
        
    protected:
        void createParticleMesh();
        void updateParticleMesh();
        
        void emitParticles(uint32_t particles);
        
        ParticleDefinition _particleDefinition;
        ParticleDefinition::PositionType _positionType;
        
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
