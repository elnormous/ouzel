// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "core/CompileConfig.h"
#include "ParticleSystem.h"
#include "core/Engine.h"
#include "SceneManager.h"
#include "files/FileSystem.h"
#include "core/Cache.h"
#include "Layer.h"
#include "scene/Camera.h"
#include "graphics/MeshBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/VertexBuffer.h"
#include "utils/Utils.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace scene
    {
        ParticleSystem::ParticleSystem()
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            updateCallback.callback = std::bind(&ParticleSystem::update, this, std::placeholders::_1);
        }

        ParticleSystem::ParticleSystem(const std::string& filename):
            ParticleSystem()
        {
            initFromFile(filename);
        }

        void ParticleSystem::draw(const Matrix4& transformMatrix,
                                  const graphics::Color& drawColor,
                                  scene::Camera* camera)
        {
            Component::draw(transformMatrix, drawColor, camera);

            if (particleCount)
            {
                if (needsMeshUpdate)
                {
                    updateParticleMesh();
                    needsMeshUpdate = false;
                }

                Matrix4 transform;

                if (positionType == ParticleDefinition::PositionType::FREE || positionType == ParticleDefinition::PositionType::RELATIVE)
                {
                    transform = camera->getViewProjection();
                }
                else if (positionType == ParticleDefinition::PositionType::GROUPED)
                {
                    transform = camera->getViewProjection() * transformMatrix;
                }

                float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = { std::begin(colorVector), std::end(colorVector) };

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = { std::begin(transform.m), std::end(transform.m) };

                sharedEngine->getRenderer()->addDrawCommand({ texture },
                                                            shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            blendState,
                                                            meshBuffer,
                                                            particleCount * 6,
                                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                            0,
                                                            camera->getRenderTarget(),
                                                            camera->getRenderViewport());
            }
        }

        void ParticleSystem::drawWireframe(const Matrix4& transformMatrix,
                                           const graphics::Color& drawColor,
                                           scene::Camera* camera)
        {
            Component::drawWireframe(transformMatrix, drawColor, camera);

            if (particleCount)
            {
                Matrix4 transform;

                if (positionType == ParticleDefinition::PositionType::FREE || positionType == ParticleDefinition::PositionType::RELATIVE)
                {
                    transform = camera->getViewProjection();
                }
                else if (positionType == ParticleDefinition::PositionType::GROUPED)
                {
                    transform = camera->getViewProjection() * transformMatrix;
                }

                float colorVector[] = { drawColor.getR(), drawColor.getG(), drawColor.getB(), drawColor.getA() };

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = { std::begin(colorVector), std::end(colorVector) };

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = { std::begin(transform.m), std::end(transform.m) };

                sharedEngine->getRenderer()->addDrawCommand({ whitePixelTexture },
                                                            shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            blendState,
                                                            meshBuffer,
                                                            particleCount * 6,
                                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                            0,
                                                            camera->getRenderTarget(),
                                                            camera->getRenderViewport(),
                                                            true);
            }
        }

        void ParticleSystem::update(float delta)
        {
            if (running && particleDefinition.emissionRate > 0.0f)
            {
                float rate = 1.0f / particleDefinition.emissionRate;

                if (particleCount < particleDefinition.maxParticles)
                {
                    emitCounter += delta;
                    if (emitCounter < 0.f)
                        emitCounter = 0.f;
                }

                uint32_t emitCount = static_cast<uint32_t>(std::min(static_cast<float>(particleDefinition.maxParticles - particleCount), emitCounter / rate));
                emitParticles(emitCount);
                emitCounter -= rate * emitCount;

                elapsed += delta;
                if (elapsed < 0.f)
                    elapsed = 0.f;
                if (particleDefinition.duration >= 0.0f && particleDefinition.duration < elapsed)
                {
                    finished = true;
                    stop();
                }
            }
            else if (active && !particleCount)
            {
                active = false;
                sharedEngine->unscheduleUpdate(updateCallback);
                if (finishHandler) finishHandler();
            }

            if (active)
            {
                for (uint32_t counter = particleCount; counter > 0; --counter)
                {
                    size_t i = counter - 1;

                    particles[i].life -= delta;

                    if (particles[i].life >= 0.0f)
                    {
                        if (particleDefinition.emitterType == ParticleDefinition::EmitterType::GRAVITY)
                        {
                            Vector2 tmp, radial, tangential;

                            // radial acceleration
                            if (particles[i].position.x == 0.0f || particles[i].position.y == 0.0f)
                            {
                                radial = particles[i].position;
                                radial.normalize();
                            }
                            tangential = radial;
                            radial.x *= particles[i].radialAcceleration;
                            radial.y *= particles[i].radialAcceleration;

                            // tangential acceleration
                            std::swap(tangential.x, tangential.y);
                            tangential.x *= - particles[i].tangentialAcceleration;
                            tangential.y *= particles[i].tangentialAcceleration;

                            // (gravity + radial + tangential) * delta
                            tmp.x = radial.x + tangential.x + particleDefinition.gravity.x;
                            tmp.y = radial.y + tangential.y + particleDefinition.gravity.y;
                            tmp.x *= delta;
                            tmp.y *= delta;

                            particles[i].direction.x += tmp.x;
                            particles[i].direction.y += tmp.y;

                            // this is cocos2d-x v3.0
                            // if (configName.length()>0 && yCoordFlipped != -1)

                            // this is cocos2d-x v3.0
                            tmp.x = particles[i].direction.x * delta * particleDefinition.yCoordFlipped;
                            tmp.y = particles[i].direction.y * delta * particleDefinition.yCoordFlipped;
                            particles[i].position.x += tmp.x;
                            particles[i].position.y += tmp.y;
                        }
                        else
                        {
                            particles[i].angle += particles[i].degreesPerSecond * delta;
                            particles[i].radius += particles[i].deltaRadius * delta;
                            particles[i].position.x = -cosf(particles[i].angle) * particles[i].radius;
                            particles[i].position.y = -sinf(particles[i].angle) * particles[i].radius * particleDefinition.yCoordFlipped;
                        }

                        //color r,g,b,a
                        particles[i].colorRed += particles[i].deltaColorRed * delta;
                        particles[i].colorGreen += particles[i].deltaColorGreen * delta;
                        particles[i].colorBlue += particles[i].deltaColorBlue * delta;
                        particles[i].colorAlpha += particles[i].deltaColorAlpha * delta;

                        //size
                        particles[i].size += (particles[i].deltaSize * delta);
                        particles[i].size = std::max(0.0f, particles[i].size);

                        //angle
                        particles[i].rotation += particles[i].deltaRotation * delta;
                    }
                    else
                    {
                        particles[i] = particles[particleCount - 1];
                        particleCount--;
                    }
                }

                // Update bounding box
                boundingBox.reset();

                if (positionType == ParticleDefinition::PositionType::FREE || positionType == ParticleDefinition::PositionType::RELATIVE)
                {
                    if (node)
                    {
                        const Matrix4& inverseTransform = node->getInverseTransform();

                        for (uint32_t i = 0; i < particleCount; i++)
                        {
                            Vector3 position = particles[i].position;
                            inverseTransform.transformPoint(position);
                            boundingBox.insertPoint(Vector2(position.x, position.y));
                        }
                    }
                }
                else if (particleDefinition.positionType == ParticleDefinition::PositionType::GROUPED)
                {
                    for (uint32_t i = 0; i < particleCount; i++)
                    {
                        boundingBox.insertPoint(particles[i].position);
                    }
                }

                needsMeshUpdate = true;
            }
        }

        bool ParticleSystem::initFromFile(const std::string& filename)
        {
            ParticleDefinitionPtr newParticleDefinition = sharedEngine->getCache()->getParticleDefinition(filename);

            if (!newParticleDefinition)
            {
                return false;
            }

            particleDefinition = *newParticleDefinition;
            positionType = particleDefinition.positionType;
            texture = sharedEngine->getCache()->getTexture(particleDefinition.textureFilename);

            if (!texture)
            {
                return false;
            }

            createParticleMesh();
            resume();

            return true;
        }

        void ParticleSystem::resume()
        {
            if (!running)
            {
                finished = false;
                running = true;

                if (!active)
                {
                    active = true;
                    sharedEngine->scheduleUpdate(updateCallback);
                }
            }
        }

        void ParticleSystem::stop()
        {
            running = false;
        }

        void ParticleSystem::reset()
        {
            emitCounter = 0.0f;
            elapsed = 0.0f;
            particleCount = 0;
            finished = false;
        }

        bool ParticleSystem::createParticleMesh()
        {
            indices.reserve(particleDefinition.maxParticles * 6);
            vertices.reserve(particleDefinition.maxParticles * 4);

            for (uint16_t i = 0; i < particleDefinition.maxParticles; ++i)
            {
                indices.push_back(i * 4 + 0);
                indices.push_back(i * 4 + 1);
                indices.push_back(i * 4 + 2);
                indices.push_back(i * 4 + 1);
                indices.push_back(i * 4 + 3);
                indices.push_back(i * 4 + 2);

                vertices.push_back(graphics::VertexPCT(Vector3(-1.0f, -1.0f, 0.0f), graphics::Color::WHITE, Vector2(0.0f, 1.0f)));
                vertices.push_back(graphics::VertexPCT(Vector3(1.0f, -1.0f, 0.0f), graphics::Color::WHITE, Vector2(1.0f, 1.0f)));
                vertices.push_back(graphics::VertexPCT(Vector3(-1.0f, 1.0f, 0.0f),  graphics::Color::WHITE, Vector2(0.0f, 0.0f)));
                vertices.push_back(graphics::VertexPCT(Vector3(1.0f, 1.0f, 0.0f),  graphics::Color::WHITE, Vector2(1.0f, 0.0f)));
            }

            indexBuffer = sharedEngine->getRenderer()->createIndexBuffer();
            indexBuffer->initFromBuffer(indices.data(), sizeof(uint16_t),
                                        static_cast<uint32_t>(indices.size()), false);

            vertexBuffer = sharedEngine->getRenderer()->createVertexBuffer();
            vertexBuffer->initFromBuffer(vertices.data(), graphics::VertexPCT::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), true);

            meshBuffer = sharedEngine->getRenderer()->createMeshBuffer();
            meshBuffer->init(indexBuffer, vertexBuffer);

            particles.resize(particleDefinition.maxParticles);

            return true;
        }

        bool ParticleSystem::updateParticleMesh()
        {
            if (node)
            {
                for (uint32_t counter = particleCount; counter > 0; --counter)
                {
                    size_t i = counter - 1;

                    Vector2 position;

                    if (positionType == ParticleDefinition::PositionType::FREE)
                    {
                        position = particles[i].position;
                    }
                    else if (positionType == ParticleDefinition::PositionType::RELATIVE)
                    {
                        position = node->getPosition() + particles[i].position;
                    }

                    float size_2 = particles[i].size / 2.0f;
                    Vector2 v1(-size_2, -size_2);
                    Vector2 v2(size_2, size_2);

                    float r = -degToRad(particles[i].rotation);
                    float cr = cosf(r);
                    float sr = sinf(r);

                    Vector2 a(v1.x * cr - v1.y * sr, v1.x * sr + v1.y * cr);
                    Vector2 b(v2.x * cr - v1.y * sr, v2.x * sr + v1.y * cr);
                    Vector2 c(v2.x * cr - v2.y * sr, v2.x * sr + v2.y * cr);
                    Vector2 d(v1.x * cr - v2.y * sr, v1.x * sr + v2.y * cr);

                    graphics::Color color(static_cast<uint8_t>(particles[i].colorRed * 255),
                                       static_cast<uint8_t>(particles[i].colorGreen * 255),
                                       static_cast<uint8_t>(particles[i].colorBlue * 255),
                                       static_cast<uint8_t>(particles[i].colorAlpha * 255));

                    vertices[i * 4 + 0].position = a + position;
                    vertices[i * 4 + 0].color = color;

                    vertices[i * 4 + 1].position = b + position;
                    vertices[i * 4 + 1].color = color;

                    vertices[i * 4 + 2].position = d + position;
                    vertices[i * 4 + 2].color = color;

                    vertices[i * 4 + 3].position = c + position;
                    vertices[i * 4 + 3].color = color;
                }

                if (!vertexBuffer->setData(vertices.data(), static_cast<uint32_t>(vertices.size())))
                {
                    return false;
                }
            }

            return true;
        }

        void ParticleSystem::emitParticles(uint32_t count)
        {
            if (particleCount + count > particleDefinition.maxParticles)
            {
                count = particleDefinition.maxParticles - particleCount;
            }

            if (count)
            {
                if (node)
                {
                    Vector2 position;

                    if (positionType == ParticleDefinition::PositionType::FREE)
                    {
                        position = node->convertLocalToWorld(Vector2::ZERO);
                    }
                    else if (positionType == ParticleDefinition::PositionType::RELATIVE)
                    {
                        position = node->convertLocalToWorld(Vector2::ZERO) - node->getPosition();
                    }

                    for (uint32_t i = particleCount; i < particleCount + count; ++i)
                    {
                        if (particleDefinition.emitterType == ParticleDefinition::EmitterType::GRAVITY)
                        {
                            particles[i].life = fmaxf(particleDefinition.particleLifespan + particleDefinition.particleLifespanVariance * randomf(-1.0f, 1.0f), 0.0f);

                            particles[i].position = particleDefinition.sourcePosition + position + Vector2(particleDefinition.sourcePositionVariance.x * randomf(-1.0f, 1.0f),
                                                                                                             particleDefinition.sourcePositionVariance.y * randomf(-1.0f, 1.0f));

                            particles[i].size = fmaxf(particleDefinition.startParticleSize + particleDefinition.startParticleSizeVariance * randomf(-1.0f, 1.0f), 0.0f);

                            float finishSize = fmaxf(particleDefinition.finishParticleSize + particleDefinition.finishParticleSizeVariance * randomf(-1.0f, 1.0f), 0.0f);
                            particles[i].deltaSize = (finishSize - particles[i].size) / particles[i].life;

                            particles[i].colorRed = clamp(particleDefinition.startColorRed + particleDefinition.startColorRedVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            particles[i].colorGreen = clamp(particleDefinition.startColorGreen + particleDefinition.startColorGreenVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            particles[i].colorBlue = clamp(particleDefinition.startColorBlue + particleDefinition.startColorBlueVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            particles[i].colorAlpha = clamp(particleDefinition.startColorAlpha + particleDefinition.startColorAlphaVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);

                            float finishColorRed = clamp(particleDefinition.finishColorRed + particleDefinition.finishColorRedVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            float finishColorGreen = clamp(particleDefinition.finishColorGreen + particleDefinition.finishColorGreenVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            float finishColorBlue = clamp(particleDefinition.finishColorBlue + particleDefinition.finishColorBlueVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);
                            float finishColorAlpha = clamp(particleDefinition.finishColorAlpha + particleDefinition.finishColorAlphaVariance * randomf(-1.0f, 1.0f), 0.0f, 1.0f);

                            particles[i].deltaColorRed = (finishColorRed - particles[i].colorRed) / particles[i].life;
                            particles[i].deltaColorGreen = (finishColorGreen - particles[i].colorGreen) / particles[i].life;
                            particles[i].deltaColorBlue = (finishColorBlue - particles[i].colorBlue) / particles[i].life;
                            particles[i].deltaColorAlpha = (finishColorAlpha - particles[i].colorAlpha) / particles[i].life;

                            //_particles[i].finishColor = finishColor;

                            particles[i].rotation = particleDefinition.startRotation + particleDefinition.startRotationVariance * randomf(-1.0f, 1.0f);

                            float finishRotation = particleDefinition.finishRotation + particleDefinition.finishRotationVariance * randomf(-1.0f, 1.0f);
                            particles[i].deltaRotation = (finishRotation - particles[i].rotation) / particles[i].life;

                            particles[i].radialAcceleration = particleDefinition.radialAcceleration + particleDefinition.radialAcceleration * randomf(-1.0f, 1.0f);
                            particles[i].tangentialAcceleration = particleDefinition.tangentialAcceleration + particleDefinition.tangentialAcceleration * randomf(-1.0f, 1.0f);

                            if (particleDefinition.rotationIsDir)
                            {
                                float a = degToRad(particleDefinition.angle + particleDefinition.angleVariance * randomf(-1.0f, 1.0f));
                                Vector2 v(cosf(a), sinf(a));
                                float s = particleDefinition.speed + particleDefinition.speedVariance * randomf(-1.0f, 1.0f);
                                Vector2 dir = v * s;
                                particles[i].direction = dir;
                                particles[i].rotation = -radToDeg(dir.getAngle());
                            }
                            else
                            {
                                float a = degToRad(particleDefinition.angle + particleDefinition.angleVariance * randomf(-1.0f, 1.0f));
                                Vector2 v(cosf(a), sinf(a));
                                float s = particleDefinition.speed + particleDefinition.speedVariance * randomf(-1.0f, 1.0f);
                                Vector2 dir = v * s;
                                particles[i].direction = dir;
                            }
                        }
                        else
                        {
                            particles[i].radius = particleDefinition.maxRadius + particleDefinition.maxRadiusVariance * randomf(-1.0f, 1.0f);
                            particles[i].angle = degToRad(particleDefinition.angle + particleDefinition.angleVariance * randomf(-1.0f, 1.0f));
                            particles[i].degreesPerSecond = degToRad(particleDefinition.rotatePerSecond + particleDefinition.rotatePerSecondVariance * randomf(-1.0f, 1.0f));

                            float endRadius = particleDefinition.minRadius + particleDefinition.minRadiusVariance * randomf(-1.0f, 1.0f);
                            particles[i].deltaRadius = (endRadius - particles[i].radius) / particles[i].life;
                        }
                    }

                    particleCount += count;
                }
            }
        }
    } // namespace scene
} // namespace ouzel
