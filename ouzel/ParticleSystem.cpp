// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
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
    namespace scene
    {
        std::shared_ptr<ParticleSystem> ParticleSystem::createFromFile(const std::string& filename)
        {
            std::shared_ptr<ParticleSystem> result = std::make_shared<ParticleSystem>();

            if (!result->initFromFile(filename))
            {
                result.reset();
            }

            return result;
        }

        ParticleSystem::ParticleSystem()
        {
            _shader = sharedEngine->getCache()->getShader(video::SHADER_TEXTURE);

    #ifdef OUZEL_PLATFORM_WINDOWS
            _uniModelViewProj = 0;
    #else
            _uniModelViewProj = _shader->getVertexShaderConstantId("modelViewProj");
    #endif

            _updateCallback = std::make_shared<UpdateCallback>();
            _updateCallback->callback = std::bind(&ParticleSystem::update, this, std::placeholders::_1);
        }

        ParticleSystem::~ParticleSystem()
        {
            sharedEngine->unscheduleUpdate(_updateCallback);
        }

        void ParticleSystem::draw()
        {
            Node::draw();

            LayerPtr layer = _layer.lock();

            if (_shader && _texture && layer && _particleCount)
            {
                if (_needsMeshUpdate)
                {
                    updateParticleMesh();
                    _needsMeshUpdate = false;
                }

                sharedEngine->getRenderer()->activateTexture(_texture, 0);
                sharedEngine->getRenderer()->activateShader(_shader);

                Matrix4 transform;

                if (_positionType == ParticleDefinition::PositionType::FREE || _positionType == ParticleDefinition::PositionType::RELATIVE)
                {
                    transform = layer->getCamera()->getViewProjection();
                }
                else if (_positionType == ParticleDefinition::PositionType::GROUPED)
                {
                    transform = layer->getCamera()->getViewProjection() * getTransform();
                }

                _shader->setVertexShaderConstant(_uniModelViewProj, { transform });

                sharedEngine->getRenderer()->drawMeshBuffer(_mesh, _particleCount * 6);
            }
        }

        void ParticleSystem::update(float delta)
        {
            if (_running && _particleDefinition.emissionRate > 0.0f)
            {
                float rate = 1.0f / _particleDefinition.emissionRate;

                if (_particleCount < _particleDefinition.maxParticles)
                {
                    _emitCounter += delta;
                    if (_emitCounter < 0.f)
                        _emitCounter = 0.f;
                }

                uint32_t emitCount = static_cast<uint32_t>(fminf(_particleDefinition.maxParticles - _particleCount, _emitCounter / rate));
                emitParticles(emitCount);
                _emitCounter -= rate * emitCount;

                _elapsed += delta;
                if (_elapsed < 0.f)
                    _elapsed = 0.f;
                if (_particleDefinition.duration >= 0.0f && _particleDefinition.duration < _elapsed)
                {
                    _finished = true;
                    stop();
                }
            }
            else if (_active && !_particleCount)
            {
                _active = false;
                sharedEngine->unscheduleUpdate(_updateCallback);

                if (_finished && _removeOnFinish)
                {
                    removeFromParent();
                    return;
                }
            }

            if (_active)
            {
                for (uint32_t counter = _particleCount; counter > 0; --counter)
                {
                    size_t i = counter - 1;

                    _particles[i].life -= delta;

                    if (_particles[i].life >= 0.0f)
                    {
                        if (_particleDefinition.emitterType == ParticleDefinition::EmitterType::GRAVITY)
                        {
                            Vector2 tmp, radial, tangential;

                            // radial acceleration
                            if (_particles[i].position.x == 0.0f || _particles[i].position.y == 0.0f)
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
                            tmp.x = radial.x + tangential.x + _particleDefinition.gravity.x;
                            tmp.y = radial.y + tangential.y + _particleDefinition.gravity.y;
                            tmp.x *= delta;
                            tmp.y *= delta;

                            _particles[i].direction.x += tmp.x;
                            _particles[i].direction.y += tmp.y;

                            // this is cocos2d-x v3.0
                            // if (_configName.length()>0 && _yCoordFlipped != -1)

                            // this is cocos2d-x v3.0
                            tmp.x = _particles[i].direction.x * delta * _particleDefinition.yCoordFlipped;
                            tmp.y = _particles[i].direction.y * delta * _particleDefinition.yCoordFlipped;
                            _particles[i].position.x += tmp.x;
                            _particles[i].position.y += tmp.y;
                        }
                        else
                        {
                            _particles[i].angle += _particles[i].degreesPerSecond * delta;
                            _particles[i].radius += _particles[i].deltaRadius * delta;
                            _particles[i].position.x = -cosf(_particles[i].angle) * _particles[i].radius;
                            _particles[i].position.y = -sinf(_particles[i].angle) * _particles[i].radius * _particleDefinition.yCoordFlipped;
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

                // Update bounding box
                _boundingBox.reset();

                if (_positionType == ParticleDefinition::PositionType::FREE || _positionType == ParticleDefinition::PositionType::RELATIVE)
                {
                    const Matrix4& inverseTransform = getInverseTransform();

                    for (uint32_t i = 0; i < _particleCount; i++)
                    {
                        Vector3 position = _particles[i].position;
                        inverseTransform.transformPoint(position);
                        _boundingBox.insertPoint(Vector2(position.x, position.y));
                    }
                }
                else if (_particleDefinition.positionType == ParticleDefinition::PositionType::GROUPED)
                {
                    for (uint32_t i = 0; i < _particleCount; i++)
                    {
                        _boundingBox.insertPoint(_particles[i].position);
                    }
                }

                _needsMeshUpdate = true;
            }
        }

        bool ParticleSystem::initFromFile(const std::string& filename)
        {
            ParticleDefinitionPtr particleDefinition = sharedEngine->getCache()->getParticleDefinition(filename);

            if (!particleDefinition)
            {
                return false;
            }

            _particleDefinition = *particleDefinition;
            _positionType = _particleDefinition.positionType;
            _texture = sharedEngine->getCache()->getTexture(_particleDefinition.textureFilename);

            if (!_texture)
            {
                return false;
            }

            createParticleMesh();
            resume();

            return true;
        }

        void ParticleSystem::resume()
        {
            if (!_running)
            {
                _finished = false;
                _running = true;

                if (!_active)
                {
                    _active = true;
                    sharedEngine->scheduleUpdate(_updateCallback);
                }
            }
        }

        void ParticleSystem::stop()
        {
            _running = false;
        }

        void ParticleSystem::reset()
        {
            _emitCounter = 0.0f;
            _elapsed = 0.0f;
            _particleCount = 0;
            _finished = false;
        }

        void ParticleSystem::createParticleMesh()
        {
            _indices.reserve(_particleDefinition.maxParticles * 6);
            _vertices.reserve(_particleDefinition.maxParticles * 4);

            for (uint16_t i = 0; i < _particleDefinition.maxParticles; ++i)
            {
                _indices.push_back(i * 4 + 0);
                _indices.push_back(i * 4 + 1);
                _indices.push_back(i * 4 + 2);
                _indices.push_back(i * 4 + 1);
                _indices.push_back(i * 4 + 3);
                _indices.push_back(i * 4 + 2);

                _vertices.push_back(video::VertexPCT(Vector3(-1.0f, -1.0f, 0.0f), video::Color(255, 255, 255, 255), Vector2(0.0f, 1.0f)));
                _vertices.push_back(video::VertexPCT(Vector3(1.0f, -1.0f, 0.0f), video::Color(255, 255, 255, 255), Vector2(1.0f, 1.0f)));
                _vertices.push_back(video::VertexPCT(Vector3(-1.0f, 1.0f, 0.0f),  video::Color(255, 255, 255, 255), Vector2(0.0f, 0.0f)));
                _vertices.push_back(video::VertexPCT(Vector3(1.0f, 1.0f, 0.0f),  video::Color(255, 255, 255, 255), Vector2(1.0f, 0.0f)));
            }

            _mesh = sharedEngine->getRenderer()->createMeshBuffer(_indices.data(), sizeof(uint16_t),
                                                                           static_cast<uint32_t>(_indices.size()), false,
                                                                           _vertices.data(), sizeof(video::VertexPCT),
                                                                           static_cast<uint32_t>(_vertices.size()), true,
                                                                           video::VertexPCT::ATTRIBUTES);

            _particles.resize(_particleDefinition.maxParticles);
        }

        void ParticleSystem::updateParticleMesh()
        {
            for (uint32_t counter = _particleCount; counter > 0; --counter)
            {
                size_t i = counter - 1;

                Vector2 position;

                if (_positionType == ParticleDefinition::PositionType::FREE)
                {
                    position = _particles[i].position;
                }
                else if (_positionType == ParticleDefinition::PositionType::RELATIVE)
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

                video::Color color(static_cast<uint8_t>(_particles[i].colorRed * 255),
                                   static_cast<uint8_t>(_particles[i].colorGreen * 255),
                                   static_cast<uint8_t>(_particles[i].colorBlue * 255),
                                   static_cast<uint8_t>(_particles[i].colorAlpha * 255));

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
            if (_particleCount + particles > _particleDefinition.maxParticles)
            {
                particles = _particleDefinition.maxParticles - _particleCount;
            }

            if (particles)
            {
                if (LayerPtr layer = _layer.lock())
                {
                    Vector2 position;

                    if (_positionType == ParticleDefinition::PositionType::FREE)
                    {
                        position = convertLocalToWorld(Vector2::ZERO);
                    }
                    else if (_positionType == ParticleDefinition::PositionType::RELATIVE)
                    {
                        position = convertLocalToWorld(Vector2::ZERO) - _position;
                    }

                    for (uint32_t i = _particleCount; i < _particleCount + particles; ++i)
                    {
                        if (_particleDefinition.emitterType == ParticleDefinition::EmitterType::GRAVITY)
                        {
                            _particles[i].life = fmaxf(_particleDefinition.particleLifespan + _particleDefinition.particleLifespanVariance * randomf(-1.0f, 1.0f), 0.0f);

                            _particles[i].position = _particleDefinition.sourcePosition + position + Vector2(_particleDefinition.sourcePositionVariance.x * randomf(-1.0f, 1.0f),
                                                                                                             _particleDefinition.sourcePositionVariance.y * randomf(-1.0f, 1.0f));

                            _particles[i].size = fmaxf(_particleDefinition.startParticleSize + _particleDefinition.startParticleSizeVariance * randomf(-1.0f, 1.0f), 0.0f);

                            float finishSize = fmaxf(_particleDefinition.finishParticleSize + _particleDefinition.finishParticleSizeVariance * randomf(-1.0f, 1.0f), 0.0f);
                            _particles[i].deltaSize = (finishSize - _particles[i].size) / _particles[i].life;

                            _particles[i].colorRed = clamp(_particleDefinition.startColorRed + _particleDefinition.startColorRedVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            _particles[i].colorGreen = clamp(_particleDefinition.startColorGreen + _particleDefinition.startColorGreenVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            _particles[i].colorBlue = clamp(_particleDefinition.startColorBlue + _particleDefinition.startColorBlueVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            _particles[i].colorAlpha = clamp(_particleDefinition.startColorAlpha + _particleDefinition.startColorAlphaVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);

                            float finishColorRed = clamp(_particleDefinition.finishColorRed + _particleDefinition.finishColorRedVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            float finishColorGreen = clamp(_particleDefinition.finishColorGreen + _particleDefinition.finishColorGreenVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            float finishColorBlue = clamp(_particleDefinition.finishColorBlue + _particleDefinition.finishColorBlueVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            float finishColorAlpha = clamp(_particleDefinition.finishColorAlpha + _particleDefinition.finishColorAlphaVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);

                            _particles[i].deltaColorRed = (finishColorRed - _particles[i].colorRed) / _particles[i].life;
                            _particles[i].deltaColorGreen = (finishColorGreen - _particles[i].colorGreen) / _particles[i].life;
                            _particles[i].deltaColorBlue = (finishColorBlue - _particles[i].colorBlue) / _particles[i].life;
                            _particles[i].deltaColorAlpha = (finishColorAlpha - _particles[i].colorAlpha) / _particles[i].life;

                            //_particles[i].finishColor = finishColor;

                            _particles[i].rotation = _particleDefinition.startRotation + _particleDefinition.startRotationVariance * randomf(-1.0f, 1.0f);

                            float finishRotation = _particleDefinition.finishRotation + _particleDefinition.finishRotationVariance * randomf(-1.0f, 1.0f);
                            _particles[i].deltaRotation = (finishRotation - _particles[i].rotation) / _particles[i].life;

                            _particles[i].radialAcceleration = _particleDefinition.radialAcceleration + _particleDefinition.radialAcceleration * randomf(-1.0f, 1.0f);
                            _particles[i].tangentialAcceleration = _particleDefinition.tangentialAcceleration + _particleDefinition.tangentialAcceleration * randomf(-1.0f, 1.0f);

                            if (_particleDefinition.rotationIsDir)
                            {
                                float a = degToRad(_particleDefinition.angle + _particleDefinition.angleVariance * randomf(-1.0f, 1.0f));
                                Vector2 v(cosf(a), sinf(a));
                                float s = _particleDefinition.speed + _particleDefinition.speedVariance * randomf(-1.0f, 1.0f);
                                Vector2 dir = v * s;
                                _particles[i].direction = dir;
                                _particles[i].rotation = -radToDeg(dir.getAngle());
                            }
                            else
                            {
                                float a = degToRad(_particleDefinition.angle + _particleDefinition.angleVariance * randomf(-1.0f, 1.0f));
                                Vector2 v(cosf(a), sinf(a));
                                float s = _particleDefinition.speed + _particleDefinition.speedVariance * randomf(-1.0f, 1.0f);
                                Vector2 dir = v * s;
                                _particles[i].direction = dir;
                            }
                        }
                        else
                        {
                            _particles[i].radius = _particleDefinition.maxRadius + _particleDefinition.maxRadiusVariance * randomf(-1.0f, 1.0f);
                            _particles[i].angle = degToRad(_particleDefinition.angle + _particleDefinition.angleVariance * randomf(-1.0f, 1.0f));
                            _particles[i].degreesPerSecond = degToRad(_particleDefinition.rotatePerSecond + _particleDefinition.rotatePerSecondVariance * randomf(-1.0f, 1.0f));

                            float endRadius = _particleDefinition.minRadius + _particleDefinition.minRadiusVariance * randomf(-1.0f, 1.0f);
                            _particles[i].deltaRadius = (endRadius - _particles[i].radius) / _particles[i].life;
                        }
                    }

                    _particleCount += particles;
                }
            }
        }
    } // namespace scene
} // namespace ouzel
