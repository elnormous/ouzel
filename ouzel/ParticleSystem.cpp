// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "CompileConfig.h"
#include "ParticleSystem.h"
#include "Engine.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "File.h"
#include "Cache.h"
#include "Layer.h"
#include "Camera.h"
#include "MeshBuffer.h"
#include "Utils.h"
#include "MathUtils.h"

namespace ouzel
{
    ParticleSystem::ParticleSystem()
    {
        _shader = Engine::getInstance()->getCache()->getShader(SHADER_TEXTURE);
        
#ifdef OUZEL_PLATFORM_WINDOWS
        _uniModelViewProj = 0;
#else
        _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
#endif
    }
    
    ParticleSystem::~ParticleSystem()
    {
        
    }
    
    void ParticleSystem::draw()
    {
        Node::draw();
        
        LayerPtr layer = _layer.lock();
        
        if (_shader && _texture && layer && _particleCount)
        {
            Engine::getInstance()->getRenderer()->activateTexture(_texture, 0);
            Engine::getInstance()->getRenderer()->activateShader(_shader);
            
            Matrix4 transform;
            
            if (_positionType == PositionType::FREE || _positionType == PositionType::RELATIVE)
            {
                transform = layer->getProjection() * layer->getCamera()->getTransform();
            }
            else if (_positionType == PositionType::GROUPED)
            {
                transform = layer->getProjection() * layer->getCamera()->getTransform() * getTransform();
            }
            
            _shader->setVertexShaderConstant(_uniModelViewProj, { transform });
            
            Engine::getInstance()->getRenderer()->drawMeshBuffer(_mesh, _particleCount * 6);
        }
    }
    
    void ParticleSystem::update(float delta)
    {
        Node::update(delta);
        
        if (_active && _emissionRate)
        {
            float rate = 1.0f / _emissionRate;
            
            if (_particleCount < _maxParticles)
            {
                _emitCounter += delta;
                if (_emitCounter < 0.f)
                    _emitCounter = 0.f;
            }
            
            int emitCount = fminf(_maxParticles - _particleCount, _emitCounter / rate);
            emitParticles(emitCount);
            _emitCounter -= rate * emitCount;
            
            _elapsed += delta;
            if (_elapsed < 0.f)
                _elapsed = 0.f;
            if (_duration >= 0.0f && _duration < _elapsed)
            {
                stop();
            }
        }
        
        // TODO: calculate bounding box
        
        for (int32_t i = _particleCount - 1; i >= 0; --i)
        {
            _particles[i].life -= delta;
            
            if (_particles[i].life >= 0.0f)
            {
                if (_emitterType == EmitterType::GRAVITY)
                {
                    Vector2 tmp, radial, tangential;
                    
                    // radial acceleration
                    if (_particles[i].position.x || _particles[i].position.y)
                    {
                        radial = _particles[i].position;
                        radial.normalize();
                    }
                    tangential = radial;
                    radial.x *= _particles[i].radialAcceleration;
                    radial.y *= _particles[i].radialAcceleration;
                    
                    // tangential acceleration
                    std::swap(tangential.x, tangential.y);
                    tangential.x *= - _particles[i].tangentialAcceleration;
                    tangential.y *= _particles[i].tangentialAcceleration;
                    
                    // (gravity + radial + tangential) * delta
                    tmp.x = radial.x + tangential.x + _gravity.x;
                    tmp.y = radial.y + tangential.y + _gravity.y;
                    tmp.x *= delta;
                    tmp.y *= delta;
                    
                    _particles[i].direction.x += tmp.x;
                    _particles[i].direction.y += tmp.y;
                    
                    // this is cocos2d-x v3.0
                    // if (_configName.length()>0 && _yCoordFlipped != -1)
                    
                    // this is cocos2d-x v3.0
                    tmp.x = _particles[i].direction.x * delta * _yCoordFlipped;
                    tmp.y = _particles[i].direction.y * delta * _yCoordFlipped;
                    _particles[i].position.x += tmp.x;
                    _particles[i].position.y += tmp.y;
                }
                else
                {
                    _particles[i].angle += _particles[i].degreesPerSecond * delta;
                    _particles[i].radius += _particles[i].deltaRadius * delta;
                    _particles[i].position.x = -cosf(_particles[i].angle) * _particles[i].radius;
                    _particles[i].position.y = -sinf(_particles[i].angle) * _particles[i].radius * _yCoordFlipped;
                }
                
                //color r,g,b,a
                _particles[i].colorRed += _particles[i].deltaColorRed * delta;
                _particles[i].colorGreen += _particles[i].deltaColorGreen * delta;
                _particles[i].colorBlue += _particles[i].deltaColorBlue * delta;
                _particles[i].colorAlpha += _particles[i].deltaColorAlpha * delta;
                
                //size
                _particles[i].size += (_particles[i].deltaSize * delta);
                _particles[i].size = fmaxf(0.0f, _particles[i].size);
                
                //angle
                _particles[i].rotation += _particles[i].deltaRotation * delta;
            }
            else
            {
                _particles[i] = _particles[_particleCount - 1];
                _particleCount--;
            }
        }
        
        updateParticleMesh();
    }
    
    bool ParticleSystem::initFromFile(const std::string& filename)
    {
        File file(filename, File::Mode::READ, false);
        
        if (file.isOpen())
        {
            rapidjson::FileReadStream is(file.getFile().get(), TEMP_BUFFER, sizeof(TEMP_BUFFER));
            
            rapidjson::Document document;
            document.ParseStream<0>(is);
            
            if (document.HasParseError())
            {
                return false;
            }
            
            if (document.HasMember("blendFuncSource")) _blendFuncSource = document["blendFuncSource"].GetInt();
            if (document.HasMember("blendFuncDestination")) _blendFuncDestination = document["blendFuncDestination"].GetInt();
            
            
            if (document.HasMember("emitterType"))
            {
                uint32_t emitterType = document["emitterType"].GetInt();
                
                switch (emitterType)
                {
                    case 0: _emitterType = EmitterType::GRAVITY; break;
                    case 1: _emitterType = EmitterType::RADIUS; break;
                }
            }
            
            if (document.HasMember("maxParticles")) _maxParticles = document["maxParticles"].GetInt();
            
            if (document.HasMember("duration")) _duration = static_cast<float>(document["duration"].GetDouble());
            if (document.HasMember("particleLifespan")) _particleLifespan = static_cast<float>(document["particleLifespan"].GetDouble());
            if (document.HasMember("particleLifespanVariance")) _particleLifespanVariance = static_cast<float>(document["particleLifespanVariance"].GetDouble());
            
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
            if (document.HasMember("rotationStart")) _startRotation = static_cast<float>(document["rotationStart"].GetDouble());
            if (document.HasMember("rotationStartVariance")) _startRotationVariance = static_cast<float>(document["rotationStartVariance"].GetDouble());
            if (document.HasMember("rotationEnd")) _finishRotation = static_cast<float>(document["rotationEnd"].GetDouble());
            if (document.HasMember("rotationEndVariance")) _finishRotationVariance = static_cast<float>(document["rotationEndVariance"].GetDouble());
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
            
            if (document.HasMember("rotationIsDir")) _rotationIsDir = static_cast<float>(document["rotationIsDir"].GetBool());
            
            if (document.HasMember("gravityx")) _gravity.x = static_cast<float>(document["gravityx"].GetDouble());
            if (document.HasMember("gravityy")) _gravity.y = static_cast<float>(document["gravityy"].GetDouble());
            
            if (document.HasMember("startColorRed")) _startColorRed = static_cast<float>(document["startColorRed"].GetDouble());
            if (document.HasMember("startColorGreen")) _startColorGreen = static_cast<float>(document["startColorGreen"].GetDouble());
            if (document.HasMember("startColorBlue")) _startColorBlue = static_cast<float>(document["startColorBlue"].GetDouble());
            if (document.HasMember("startColorAlpha")) _startColorAlpha = static_cast<float>(document["startColorAlpha"].GetDouble());
            
            if (document.HasMember("startColorVarianceRed")) _startColorRedVariance = static_cast<float>(document["startColorVarianceRed"].GetDouble());
            if (document.HasMember("startColorVarianceGreen")) _startColorGreenVariance = static_cast<float>(document["startColorVarianceGreen"].GetDouble());
            if (document.HasMember("startColorVarianceBlue")) _startColorBlueVariance = static_cast<float>(document["startColorVarianceBlue"].GetDouble());
            if (document.HasMember("startColorVarianceAlpha")) _startColorAlphaVariance = static_cast<float>(document["startColorVarianceAlpha"].GetDouble());
            
            if (document.HasMember("finishColorRed")) _finishColorRed = static_cast<float>(document["finishColorRed"].GetDouble());
            if (document.HasMember("finishColorGreen")) _finishColorGreen = static_cast<float>(document["finishColorGreen"].GetDouble());
            if (document.HasMember("finishColorBlue")) _finishColorBlue = static_cast<float>(document["finishColorBlue"].GetDouble());
            if (document.HasMember("finishColorAlpha")) _finishColorAlpha = static_cast<float>(document["finishColorAlpha"].GetDouble());
            
            if (document.HasMember("finishColorVarianceRed")) _finishColorRedVariance = static_cast<float>(document["finishColorVarianceRed"].GetDouble());
            if (document.HasMember("finishColorVarianceGreen")) _finishColorGreenVariance = static_cast<float>(document["finishColorVarianceGreen"].GetDouble());
            if (document.HasMember("finishColorVarianceBlue")) _finishColorBlueVariance = static_cast<float>(document["finishColorVarianceBlue"].GetDouble());
            if (document.HasMember("finishColorVarianceAlpha")) _finishColorAlphaVariance = static_cast<float>(document["finishColorVarianceAlpha"].GetDouble());
            
            if (document.HasMember("textureFileName")) _textureFilename = document["textureFileName"].GetString();
        }
        else
        {
            return false;
        }
        
        _emissionRate = static_cast<float>(_maxParticles) / _particleLifespan;
        
        _texture = Engine::getInstance()->getCache()->getTexture(_textureFilename);
        
        if (!_texture)
        {
            return false;
        }
        
        createParticleMesh();
        
        return true;
    }
    
    void ParticleSystem::resume()
    {
        _active = true;
    }
    
    void ParticleSystem::stop()
    {
        _active = false;
    }
    
    void ParticleSystem::reset()
    {
        _emitCounter = 0.0f;
        _elapsed = 0.0f;
        _particleCount = 0;
    }
    
    void ParticleSystem::createParticleMesh()
    {
        _indices.reserve(_maxParticles * 6);
        _vertices.reserve(_maxParticles * 4);
        
        for (int32_t i = 0; i < _maxParticles; ++i)
        {
            _indices.push_back(i * 4 + 0);
            _indices.push_back(i * 4 + 1);
            _indices.push_back(i * 4 + 2);
            _indices.push_back(i * 4 + 1);
            _indices.push_back(i * 4 + 3);
            _indices.push_back(i * 4 + 2);
            
            _vertices.push_back(VertexPCT(Vector3(-1.0f, -1.0f, 0.0f), Color(255, 255, 255, 255), Vector2(0.0f, 1.0f)));
            _vertices.push_back(VertexPCT(Vector3(1.0f, -1.0f, 0.0f), Color(255, 255, 255, 255), Vector2(1.0f, 1.0f)));
            _vertices.push_back(VertexPCT(Vector3(-1.0f, 1.0f, 0.0f),  Color(255, 255, 255, 255), Vector2(0.0f, 0.0f)));
            _vertices.push_back(VertexPCT(Vector3(1.0f, 1.0f, 0.0f),  Color(255, 255, 255, 255), Vector2(1.0f, 0.0f)));
        }

        _mesh = Engine::getInstance()->getRenderer()->createMeshBuffer(_indices.data(), sizeof(uint16_t),
                                                                       static_cast<uint32_t>(_indices.size()), false,
                                                                       _vertices.data(), sizeof(VertexPCT),
                                                                       static_cast<uint32_t>(_vertices.size()), true,
                                                                       VertexPCT::ATTRIBUTES);
        
        _particles.resize(_maxParticles);
    }
    
    void ParticleSystem::updateParticleMesh()
    {
        for (int32_t i = _particleCount - 1; i >= 0; --i)
        {
            Vector2 position;
            
            if (_positionType == PositionType::FREE)
            {
                position = _particles[i].position;
            }
            else if (_positionType == PositionType::RELATIVE)
            {
                position = _position + _particles[i].position;
            }
            
            float size_2 = _particles[i].size / 2.0f;
            Vector2 v1(-size_2, -size_2);
            Vector2 v2(size_2, size_2);
            
            float r = -degToRad(_particles[i].rotation);
            float cr = cosf(r);
            float sr = sinf(r);
            
            Vector2 a(v1.x * cr - v1.y * sr, v1.x * sr + v1.y * cr);
            Vector2 b(v2.x * cr - v1.y * sr, v2.x * sr + v1.y * cr);
            Vector2 c(v2.x * cr - v2.y * sr, v2.x * sr + v2.y * cr);
            Vector2 d(v1.x * cr - v2.y * sr, v1.x * sr + v2.y * cr);
            
            Color color(_particles[i].colorRed * 255,
                        _particles[i].colorGreen * 255,
                        _particles[i].colorBlue * 255,
                        _particles[i].colorAlpha * 255);
            
            _vertices[i * 4 + 0].position = a + position;
            _vertices[i * 4 + 0].color = color;
            
            _vertices[i * 4 + 1].position = b + position;
            _vertices[i * 4 + 1].color = color;
            
            _vertices[i * 4 + 2].position = d + position;
            _vertices[i * 4 + 2].color = color;
            
            _vertices[i * 4 + 3].position = c + position;
            _vertices[i * 4 + 3].color = color;
        }
        
        _mesh->uploadVertices(_vertices.data(), static_cast<uint32_t>(_vertices.size()));
    }
    
    void ParticleSystem::emitParticles(uint32_t particles)
    {
        if (_particleCount + particles > _maxParticles)
        {
            particles = _maxParticles - _particleCount;
        }
        
        if (particles)
        {
            Vector2 position;
            if (LayerPtr layer = _layer.lock())
            {
                if (_positionType == PositionType::FREE)
                {
                    position = convertLocalToWorld(Vector2::zero());
                }
                else if (_positionType == PositionType::RELATIVE)
                {
                    position = convertLocalToWorld(Vector2::zero()) - _position;
                }
            }
            
            for (uint32_t i = _particleCount; i < _particleCount + particles; ++i)
            {
                if (_emitterType == EmitterType::GRAVITY)
                {
                    _particles[i].life = fmaxf(_particleLifespan + _particleLifespanVariance * RANDOM_MINUS1_1(), 0.0f);
                    
                    _particles[i].position = _sourcePosition + position + Vector2(_sourcePositionVariance.x * RANDOM_MINUS1_1(),
                                                                                  _sourcePositionVariance.y * RANDOM_MINUS1_1());
                    
                    _particles[i].size = fmaxf(_startParticleSize + _startParticleSizeVariance * RANDOM_MINUS1_1(), 0.0f);
                    
                    float finishSize = fmaxf(_finishParticleSize + _finishParticleSizeVariance * RANDOM_MINUS1_1(), 0.0f);
                    _particles[i].deltaSize = (finishSize - _particles[i].size) / _particles[i].life;
                    
                    _particles[i].colorRed = clamp(_startColorRed + _startColorRedVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    _particles[i].colorGreen = clamp(_startColorGreen + _startColorGreenVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    _particles[i].colorBlue = clamp(_startColorBlue + _startColorBlueVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    _particles[i].colorAlpha = clamp(_startColorAlpha + _startColorAlphaVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    
                    float finishColorRed = clamp(_finishColorRed + _finishColorRedVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    float finishColorGreen = clamp(_finishColorGreen + _finishColorGreenVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    float finishColorBlue = clamp(_finishColorBlue + _finishColorBlueVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    float finishColorAlpha = clamp(_finishColorAlpha + _finishColorAlphaVariance * RANDOM_MINUS1_1(), 0.0f, 1.0f);
                    
                    _particles[i].deltaColorRed = (finishColorRed - _particles[i].colorRed) / _particles[i].life;
                    _particles[i].deltaColorGreen = (finishColorGreen - _particles[i].colorGreen) / _particles[i].life;
                    _particles[i].deltaColorBlue = (finishColorBlue - _particles[i].colorBlue) / _particles[i].life;
                    _particles[i].deltaColorAlpha = (finishColorAlpha - _particles[i].colorAlpha) / _particles[i].life;
                    
                    //_particles[i].finishColor = finishColor;
                    
                    _particles[i].rotation = _startRotation + _startRotationVariance * RANDOM_MINUS1_1();
                    
                    float finishRotation = _finishRotation + _finishRotationVariance * RANDOM_MINUS1_1();
                    _particles[i].deltaRotation = (finishRotation - _particles[i].rotation) / _particles[i].life;
                    
                    _particles[i].radialAcceleration = _radialAcceleration + _radialAcceleration * RANDOM_MINUS1_1();
                    _particles[i].tangentialAcceleration = _tangentialAcceleration + _tangentialAcceleration * RANDOM_MINUS1_1();
                    
                    if (_rotationIsDir)
                    {
                        float a = degToRad(_angle + _angleVariance * RANDOM_MINUS1_1());
                        Vector2 v(cosf(a), sinf(a));
                        float s = _speed + _speedVariance * RANDOM_MINUS1_1();
                        Vector2 dir = v * s;
                        _particles[i].direction = dir;
                        _particles[i].rotation = -radToDeg(dir.getAngle());
                    }
                    else
                    {
                        float a = degToRad(_angle + _angleVariance * RANDOM_MINUS1_1());
                        Vector2 v(cosf(a), sinf(a));
                        float s = _speed + _speedVariance * RANDOM_MINUS1_1();
                        Vector2 dir = v * s;
                        _particles[i].direction = dir;
                    }
                }
                else
                {
                    _particles[i].radius = _maxRadius + _maxRadiusVariance * RANDOM_MINUS1_1();
                    _particles[i].angle = degToRad(_angle + _angleVariance * RANDOM_MINUS1_1());
                    _particles[i].degreesPerSecond = degToRad(_rotatePerSecond + _rotatePerSecondVariance * RANDOM_MINUS1_1());
                    
                    float endRadius = _minRadius + _minRadiusVariance * RANDOM_MINUS1_1();
                    _particles[i].deltaRadius = (endRadius - _particles[i].radius) / _particles[i].life;
                }
            }
            
            _particleCount += particles;
        }
    }
}
