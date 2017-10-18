// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "core/Setup.h"
#include "ParticleSystem.hpp"
#include "core/Engine.hpp"
#include "SceneManager.hpp"
#include "files/FileSystem.hpp"
#include "core/Cache.hpp"
#include "Layer.hpp"
#include "scene/Camera.hpp"
#include "graphics/MeshBufferResource.hpp"
#include "graphics/BufferResource.hpp"
#include "utils/Utils.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace scene
    {
        ParticleSystem::ParticleSystem():
            Component(TYPE)
        {
            shader = sharedEngine->getCache()->getShader(graphics::SHADER_TEXTURE);
            blendState = sharedEngine->getCache()->getBlendState(graphics::BLEND_ALPHA);
            whitePixelTexture = sharedEngine->getCache()->getTexture(graphics::TEXTURE_WHITE_PIXEL);

            updateCallback.callback = std::bind(&ParticleSystem::update, this, std::placeholders::_1);
        }

        ParticleSystem::ParticleSystem(const std::string& filename):
            ParticleSystem()
        {
            init(filename);
        }

        void ParticleSystem::draw(const Matrix4& transformMatrix,
                                  float opacity,
                                  const Matrix4& renderViewProjection,
                                  const std::shared_ptr<graphics::Texture>& renderTarget,
                                  const Rectangle& renderViewport,
                                  bool depthWrite,
                                  bool depthTest,
                                  bool wireframe,
                                  bool scissorTest,
                                  const Rectangle& scissorRectangle)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            renderTarget,
                            renderViewport,
                            depthWrite,
                            depthTest,
                            wireframe,
                            scissorTest,
                            scissorRectangle);

            if (particleCount)
            {
                if (needsMeshUpdate)
                {
                    updateParticleMesh();
                    needsMeshUpdate = false;
                }

                Matrix4 transform;

                if (particleDefinition.positionType == ParticleDefinition::PositionType::FREE ||
                    particleDefinition.positionType == ParticleDefinition::PositionType::PARENT)
                {
                    transform = renderViewProjection;
                }
                else if (particleDefinition.positionType == ParticleDefinition::PositionType::GROUPED)
                {
                    transform = renderViewProjection * transformMatrix;
                }

                float colorVector[] = {1.0f, 1.0f, 1.0f, opacity};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(transform.m), std::end(transform.m)};

                sharedEngine->getRenderer()->addDrawCommand({wireframe ? whitePixelTexture : texture},
                                                            shader,
                                                            pixelShaderConstants,
                                                            vertexShaderConstants,
                                                            blendState,
                                                            meshBuffer,
                                                            particleCount * 6,
                                                            graphics::Renderer::DrawMode::TRIANGLE_LIST,
                                                            0,
                                                            renderTarget,
                                                            renderViewport,
                                                            depthWrite,
                                                            depthTest,
                                                            wireframe,
                                                            scissorTest,
                                                            scissorRectangle,
                                                            graphics::Renderer::CullMode::NONE);
            }
        }

        void ParticleSystem::update(float delta)
        {
            timeSinceUpdate += delta;

            static const float UPDATE_STEP = 1.0f / 60.0f;
            bool needsBoundingBoxUpdate = false;

            while (timeSinceUpdate >= UPDATE_STEP)
            {
                timeSinceUpdate -= UPDATE_STEP;

                if (running && particleDefinition.emissionRate > 0.0f)
                {
                    float rate = 1.0f / particleDefinition.emissionRate;

                    if (particleCount < particleDefinition.maxParticles)
                    {
                        emitCounter += UPDATE_STEP;
                        if (emitCounter < 0.f)
                            emitCounter = 0.f;
                    }

                    uint32_t emitCount = static_cast<uint32_t>(std::min(static_cast<float>(particleDefinition.maxParticles - particleCount), emitCounter / rate));
                    emitParticles(emitCount);
                    emitCounter -= rate * emitCount;

                    elapsed += UPDATE_STEP;
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
                    updateCallback.remove();

                    Event finishEvent;
                    finishEvent.type = Event::Type::ANIMATION_FINISH;
                    finishEvent.animationEvent.component = this;
                    sharedEngine->getEventDispatcher()->postEvent(finishEvent);

                    return;
                }

                if (active)
                {
                    for (uint32_t counter = particleCount; counter > 0; --counter)
                    {
                        size_t i = counter - 1;

                        particles[i].life -= UPDATE_STEP;

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
                                radial *= particles[i].radialAcceleration;

                                // tangential acceleration
                                std::swap(tangential.x, tangential.y);
                                tangential.x *= - particles[i].tangentialAcceleration;
                                tangential.y *= particles[i].tangentialAcceleration;

                                // (gravity + radial + tangential) * UPDATE_STEP
                                tmp.x = radial.x + tangential.x + particleDefinition.gravity.x;
                                tmp.y = radial.y + tangential.y + particleDefinition.gravity.y;
                                tmp.x *= UPDATE_STEP;
                                tmp.y *= UPDATE_STEP;

                                particles[i].direction.x += tmp.x;
                                particles[i].direction.y += tmp.y;
                                tmp.x = particles[i].direction.x * UPDATE_STEP * particleDefinition.yCoordFlipped;
                                tmp.y = particles[i].direction.y * UPDATE_STEP * particleDefinition.yCoordFlipped;
                                particles[i].position.x += tmp.x;
                                particles[i].position.y += tmp.y;
                            }
                            else
                            {
                                particles[i].angle += particles[i].degreesPerSecond * UPDATE_STEP;
                                particles[i].radius += particles[i].deltaRadius * UPDATE_STEP;
                                particles[i].position.x = -cosf(particles[i].angle) * particles[i].radius;
                                particles[i].position.y = -sinf(particles[i].angle) * particles[i].radius * particleDefinition.yCoordFlipped;
                            }

                            //color r,g,b,a
                            particles[i].colorRed += particles[i].deltaColorRed * UPDATE_STEP;
                            particles[i].colorGreen += particles[i].deltaColorGreen * UPDATE_STEP;
                            particles[i].colorBlue += particles[i].deltaColorBlue * UPDATE_STEP;
                            particles[i].colorAlpha += particles[i].deltaColorAlpha * UPDATE_STEP;

                            //size
                            particles[i].size += (particles[i].deltaSize * UPDATE_STEP);
                            particles[i].size = std::max(0.0f, particles[i].size);

                            //angle
                            particles[i].rotation += particles[i].deltaRotation * UPDATE_STEP;
                        }
                        else
                        {
                            particles[i] = particles[particleCount - 1];
                            particleCount--;
                        }
                    }

                    needsMeshUpdate = true;
                    needsBoundingBoxUpdate = true;
                }
            }

            if (needsBoundingBoxUpdate)
            {
                // Update bounding box
                boundingBox.reset();

                if (particleDefinition.positionType == ParticleDefinition::PositionType::FREE ||
                    particleDefinition.positionType == ParticleDefinition::PositionType::PARENT)
                {
                    if (actor)
                    {
                        const Matrix4& inverseTransform = actor->getInverseTransform();

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
            }
        }

        bool ParticleSystem::init(const ParticleDefinition& newParticleDefinition)
        {
            particleDefinition = newParticleDefinition;

            texture = particleDefinition.texture;

            if (!texture)
            {
                return false;
            }

            createParticleMesh();
            resume();

            return true;
        }

        bool ParticleSystem::init(const std::string& filename)
        {
            particleDefinition = sharedEngine->getCache()->getParticleDefinition(filename);

            texture = particleDefinition.texture;

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
                    sharedEngine->scheduleUpdate(&updateCallback);
                }

                if (particleCount == 0)
                {
                    Event startEvent;
                    startEvent.type = Event::Type::ANIMATION_START;
                    startEvent.animationEvent.component = this;
                    sharedEngine->getEventDispatcher()->postEvent(startEvent);
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
            timeSinceUpdate = 0.0f;
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

                vertices.push_back(graphics::VertexPCT(Vector3(-1.0f, -1.0f, 0.0f), Color::WHITE, Vector2(0.0f, 1.0f)));
                vertices.push_back(graphics::VertexPCT(Vector3(1.0f, -1.0f, 0.0f), Color::WHITE, Vector2(1.0f, 1.0f)));
                vertices.push_back(graphics::VertexPCT(Vector3(-1.0f, 1.0f, 0.0f),  Color::WHITE, Vector2(0.0f, 0.0f)));
                vertices.push_back(graphics::VertexPCT(Vector3(1.0f, 1.0f, 0.0f),  Color::WHITE, Vector2(1.0f, 0.0f)));
            }

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), graphics::Buffer::DYNAMIC);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);

            particles.resize(particleDefinition.maxParticles);

            return true;
        }

        bool ParticleSystem::updateParticleMesh()
        {
            if (actor)
            {
                for (uint32_t counter = particleCount; counter > 0; --counter)
                {
                    size_t i = counter - 1;

                    Vector2 position;

                    if (particleDefinition.positionType == ParticleDefinition::PositionType::FREE)
                    {
                        position = particles[i].position;
                    }
                    else if (particleDefinition.positionType == ParticleDefinition::PositionType::PARENT)
                    {
                        position = actor->getPosition() + particles[i].position;
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

                    Color color(static_cast<uint8_t>(particles[i].colorRed * 255),
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

                if (!vertexBuffer->setData(vertices.data(), static_cast<uint32_t>(getVectorSize(vertices))))
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
                if (actor)
                {
                    Vector2 position;

                    if (particleDefinition.positionType == ParticleDefinition::PositionType::FREE)
                    {
                        position = actor->convertLocalToWorld(Vector2::ZERO);
                    }
                    else if (particleDefinition.positionType == ParticleDefinition::PositionType::PARENT)
                    {
                        position = actor->convertLocalToWorld(Vector2::ZERO) - actor->getPosition();
                    }

                    for (uint32_t i = particleCount; i < particleCount + count; ++i)
                    {
                        if (particleDefinition.emitterType == ParticleDefinition::EmitterType::GRAVITY)
                        {
                            particles[i].life = fmaxf(particleDefinition.particleLifespan + particleDefinition.particleLifespanVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f);

                            particles[i].position = particleDefinition.sourcePosition + position + Vector2(particleDefinition.sourcePositionVariance.x * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine),
                                                                                                             particleDefinition.sourcePositionVariance.y * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine));

                            particles[i].size = fmaxf(particleDefinition.startParticleSize + particleDefinition.startParticleSizeVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f);

                            float finishSize = fmaxf(particleDefinition.finishParticleSize + particleDefinition.finishParticleSizeVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f);
                            particles[i].deltaSize = (finishSize - particles[i].size) / particles[i].life;

                            particles[i].colorRed = clamp(particleDefinition.startColorRed + particleDefinition.startColorRedVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);
                            particles[i].colorGreen = clamp(particleDefinition.startColorGreen + particleDefinition.startColorGreenVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);
                            particles[i].colorBlue = clamp(particleDefinition.startColorBlue + particleDefinition.startColorBlueVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);
                            particles[i].colorAlpha = clamp(particleDefinition.startColorAlpha + particleDefinition.startColorAlphaVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);

                            float finishColorRed = clamp(particleDefinition.finishColorRed + particleDefinition.finishColorRedVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);
                            float finishColorGreen = clamp(particleDefinition.finishColorGreen + particleDefinition.finishColorGreenVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);
                            float finishColorBlue = clamp(particleDefinition.finishColorBlue + particleDefinition.finishColorBlueVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);
                            float finishColorAlpha = clamp(particleDefinition.finishColorAlpha + particleDefinition.finishColorAlphaVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine), 0.0f, 1.0f);

                            particles[i].deltaColorRed = (finishColorRed - particles[i].colorRed) / particles[i].life;
                            particles[i].deltaColorGreen = (finishColorGreen - particles[i].colorGreen) / particles[i].life;
                            particles[i].deltaColorBlue = (finishColorBlue - particles[i].colorBlue) / particles[i].life;
                            particles[i].deltaColorAlpha = (finishColorAlpha - particles[i].colorAlpha) / particles[i].life;

                            //_particles[i].finishColor = finishColor;

                            particles[i].rotation = particleDefinition.startRotation + particleDefinition.startRotationVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);

                            float finishRotation = particleDefinition.finishRotation + particleDefinition.finishRotationVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);
                            particles[i].deltaRotation = (finishRotation - particles[i].rotation) / particles[i].life;

                            particles[i].radialAcceleration = particleDefinition.radialAcceleration + particleDefinition.radialAcceleration * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);
                            particles[i].tangentialAcceleration = particleDefinition.tangentialAcceleration + particleDefinition.tangentialAcceleration * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);

                            if (particleDefinition.rotationIsDir)
                            {
                                float a = degToRad(particleDefinition.angle + particleDefinition.angleVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine));
                                Vector2 v(cosf(a), sinf(a));
                                float s = particleDefinition.speed + particleDefinition.speedVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);
                                Vector2 dir = v * s;
                                particles[i].direction = dir;
                                particles[i].rotation = -radToDeg(dir.getAngle());
                            }
                            else
                            {
                                float a = degToRad(particleDefinition.angle + particleDefinition.angleVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine));
                                Vector2 v(cosf(a), sinf(a));
                                float s = particleDefinition.speed + particleDefinition.speedVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);
                                Vector2 dir = v * s;
                                particles[i].direction = dir;
                            }
                        }
                        else
                        {
                            particles[i].radius = particleDefinition.maxRadius + particleDefinition.maxRadiusVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);
                            particles[i].angle = degToRad(particleDefinition.angle + particleDefinition.angleVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine));
                            particles[i].degreesPerSecond = degToRad(particleDefinition.rotatePerSecond + particleDefinition.rotatePerSecondVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine));

                            float endRadius = particleDefinition.minRadius + particleDefinition.minRadiusVariance * std::uniform_real_distribution<float>{-1.0f, 1.0f}(randomEngine);
                            particles[i].deltaRadius = (endRadius - particles[i].radius) / particles[i].life;
                        }
                    }

                    particleCount += count;
                }
            }
        }
    } // namespace scene
} // namespace ouzel
