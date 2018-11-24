// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <stdexcept>
#include "core/Setup.h"
#include "ParticleSystem.hpp"
#include "core/Engine.hpp"
#include "SceneManager.hpp"
#include "files/FileSystem.hpp"
#include "assets/Cache.hpp"
#include "Actor.hpp"
#include "Camera.hpp"
#include "Layer.hpp"
#include "utils/Errors.hpp"
#include "utils/Utils.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace scene
    {
        ParticleSystem::ParticleSystem():
            Component(CLASS)
        {
            shader = engine->getCache().getShader(SHADER_TEXTURE);
            blendState = engine->getCache().getBlendState(BLEND_ALPHA);
            whitePixelTexture = engine->getCache().getTexture(TEXTURE_WHITE_PIXEL);

            updateHandler.updateHandler = std::bind(&ParticleSystem::handleUpdate, this, std::placeholders::_1);
        }

        ParticleSystem::ParticleSystem(const ParticleSystemData& initParticleSystemData):
            ParticleSystem()
        {
            init(initParticleSystemData);
        }

        ParticleSystem::ParticleSystem(const std::string& filename):
            ParticleSystem()
        {
            init(filename);
        }

        void ParticleSystem::draw(const Matrix4& transformMatrix,
                                  float opacity,
                                  const Matrix4& renderViewProjection,
                                  bool wireframe)
        {
            Component::draw(transformMatrix,
                            opacity,
                            renderViewProjection,
                            wireframe);

            if (particleCount)
            {
                if (needsMeshUpdate)
                {
                    updateParticleMesh();
                    needsMeshUpdate = false;
                }

                Matrix4 transform;

                if (particleSystemData.positionType == ParticleSystemData::PositionType::FREE ||
                    particleSystemData.positionType == ParticleSystemData::PositionType::PARENT)
                    transform = renderViewProjection;
                else if (particleSystemData.positionType == ParticleSystemData::PositionType::GROUPED)
                    transform = renderViewProjection * transformMatrix;

                float colorVector[] = {1.0F, 1.0F, 1.0F, opacity};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(transform.m), std::end(transform.m)};

                engine->getRenderer()->setCullMode(graphics::CullMode::NONE);
                engine->getRenderer()->setPipelineState(blendState->getResource(), shader->getResource());
                engine->getRenderer()->setShaderConstants(pixelShaderConstants,
                                                          vertexShaderConstants);
                engine->getRenderer()->setTextures({wireframe ? whitePixelTexture->getResource() : texture->getResource()});
                engine->getRenderer()->draw(indexBuffer->getResource(),
                                            particleCount * 6,
                                            sizeof(uint16_t),
                                            vertexBuffer->getResource(),
                                            graphics::DrawMode::TRIANGLE_LIST,
                                            0);
            }
        }

        void ParticleSystem::update(float delta)
        {
            timeSinceUpdate += delta;

            static const float UPDATE_STEP = 1.0F / 60.0F;
            bool needsBoundingBoxUpdate = false;

            while (timeSinceUpdate >= UPDATE_STEP)
            {
                timeSinceUpdate -= UPDATE_STEP;

                if (running && particleSystemData.emissionRate > 0.0F)
                {
                    float rate = 1.0F / particleSystemData.emissionRate;

                    if (particleCount < particleSystemData.maxParticles)
                    {
                        emitCounter += UPDATE_STEP;
                        if (emitCounter < 0.0F)
                            emitCounter = 0.0F;
                    }

                    uint32_t emitCount = static_cast<uint32_t>(std::min(static_cast<float>(particleSystemData.maxParticles - particleCount), emitCounter / rate));
                    emitParticles(emitCount);
                    emitCounter -= rate * emitCount;

                    elapsed += UPDATE_STEP;
                    if (elapsed < 0.0F)
                        elapsed = 0.0F;
                    if (particleSystemData.duration >= 0.0F && particleSystemData.duration < elapsed)
                    {
                        finished = true;
                        stop();
                    }
                }
                else if (active && !particleCount)
                {
                    active = false;
                    updateHandler.remove();

                    std::unique_ptr<AnimationEvent> finishEvent(new AnimationEvent());
                    finishEvent->type = Event::Type::ANIMATION_FINISH;
                    finishEvent->component = this;
                    engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));

                    return;
                }

                if (active)
                {
                    for (uint32_t counter = particleCount; counter > 0; --counter)
                    {
                        size_t i = counter - 1;

                        particles[i].life -= UPDATE_STEP;

                        if (particles[i].life >= 0.0F)
                        {
                            if (particleSystemData.emitterType == ParticleSystemData::EmitterType::GRAVITY)
                            {
                                Vector2 tmp;
                                Vector2 radial;
                                Vector2 tangential;

                                // radial acceleration
                                if (particles[i].position.x == 0.0F || particles[i].position.y == 0.0F)
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
                                tmp.x = radial.x + tangential.x + particleSystemData.gravity.x;
                                tmp.y = radial.y + tangential.y + particleSystemData.gravity.y;
                                tmp.x *= UPDATE_STEP;
                                tmp.y *= UPDATE_STEP;

                                particles[i].direction.x += tmp.x;
                                particles[i].direction.y += tmp.y;
                                tmp.x = particles[i].direction.x * UPDATE_STEP * particleSystemData.yCoordFlipped;
                                tmp.y = particles[i].direction.y * UPDATE_STEP * particleSystemData.yCoordFlipped;
                                particles[i].position.x += tmp.x;
                                particles[i].position.y += tmp.y;
                            }
                            else
                            {
                                particles[i].angle += particles[i].degreesPerSecond * UPDATE_STEP;
                                particles[i].radius += particles[i].deltaRadius * UPDATE_STEP;
                                particles[i].position.x = -cosf(particles[i].angle) * particles[i].radius;
                                particles[i].position.y = -sinf(particles[i].angle) * particles[i].radius * particleSystemData.yCoordFlipped;
                            }

                            // color r,g,b,a
                            particles[i].colorRed += particles[i].deltaColorRed * UPDATE_STEP;
                            particles[i].colorGreen += particles[i].deltaColorGreen * UPDATE_STEP;
                            particles[i].colorBlue += particles[i].deltaColorBlue * UPDATE_STEP;
                            particles[i].colorAlpha += particles[i].deltaColorAlpha * UPDATE_STEP;

                            // size
                            particles[i].size += (particles[i].deltaSize * UPDATE_STEP);
                            particles[i].size = std::max(0.0F, particles[i].size);

                            // angle
                            particles[i].rotation += particles[i].deltaRotation * UPDATE_STEP;
                        }
                        else
                        {
                            particles[i] = particles[particleCount - 1];
                            --particleCount;
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

                if (particleSystemData.positionType == ParticleSystemData::PositionType::FREE ||
                    particleSystemData.positionType == ParticleSystemData::PositionType::PARENT)
                {
                    if (actor)
                    {
                        const Matrix4& inverseTransform = actor->getInverseTransform();

                        for (uint32_t i = 0; i < particleCount; ++i)
                        {
                            Vector3 position = Vector3(particles[i].position);
                            inverseTransform.transformPoint(position);
                            boundingBox.insertPoint(position);
                        }
                    }
                }
                else if (particleSystemData.positionType == ParticleSystemData::PositionType::GROUPED)
                {
                    for (uint32_t i = 0; i < particleCount; ++i)
                        boundingBox.insertPoint(particles[i].position);
                }
            }
        }

        bool ParticleSystem::handleUpdate(const UpdateEvent& event)
        {
            update(event.delta);
            return false;
        }

        void ParticleSystem::init(const ParticleSystemData& newParticleSystemData)
        {
            particleSystemData = newParticleSystemData;

            texture = particleSystemData.texture;

            if (!texture)
                throw std::runtime_error("Paricle system data has no texture");

            createParticleMesh();
            resume();
        }

        void ParticleSystem::init(const std::string& filename)
        {
            particleSystemData = *engine->getCache().getParticleSystemData(filename);

            texture = particleSystemData.texture;

            if (!texture)
                throw std::runtime_error("Paricle system data has no texture");

            createParticleMesh();
            resume();
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
                    engine->getEventDispatcher().addEventHandler(&updateHandler);
                }

                if (particleCount == 0)
                {
                    std::unique_ptr<AnimationEvent> startEvent(new AnimationEvent());
                    startEvent->type = Event::Type::ANIMATION_START;
                    startEvent->component = this;
                    engine->getEventDispatcher().dispatchEvent(std::move(startEvent));
                }
            }
        }

        void ParticleSystem::stop()
        {
            running = false;
        }

        void ParticleSystem::reset()
        {
            emitCounter = 0.0F;
            elapsed = 0.0F;
            timeSinceUpdate = 0.0F;
            particleCount = 0;
            finished = false;
        }

        void ParticleSystem::createParticleMesh()
        {
            indices.reserve(particleSystemData.maxParticles * 6);
            vertices.reserve(particleSystemData.maxParticles * 4);

            for (uint16_t i = 0; i < particleSystemData.maxParticles; ++i)
            {
                indices.push_back(i * 4 + 0);
                indices.push_back(i * 4 + 1);
                indices.push_back(i * 4 + 2);
                indices.push_back(i * 4 + 1);
                indices.push_back(i * 4 + 3);
                indices.push_back(i * 4 + 2);

                vertices.push_back(graphics::Vertex(Vector3(-1.0F, -1.0F, 0.0F), Color::WHITE,
                                                    Vector2(0.0F, 1.0F), Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(1.0F, -1.0F, 0.0F), Color::WHITE,
                                                    Vector2(1.0F, 1.0F), Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(-1.0F, 1.0F, 0.0F), Color::WHITE,
                                                    Vector2(0.0F, 0.0F), Vector3(0.0F, 0.0F, -1.0F)));
                vertices.push_back(graphics::Vertex(Vector3(1.0F, 1.0F, 0.0F), Color::WHITE,
                                                    Vector2(1.0F, 0.0F), Vector3(0.0F, 0.0F, -1.0F)));
            }

            indexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer());
            indexBuffer->init(graphics::Buffer::Usage::INDEX, 0, indices.data(), static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer());
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, graphics::Buffer::DYNAMIC, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));

            particles.resize(particleSystemData.maxParticles);
        }

        void ParticleSystem::updateParticleMesh()
        {
            if (actor)
            {
                for (uint32_t counter = particleCount; counter > 0; --counter)
                {
                    size_t i = counter - 1;

                    Vector2 position;

                    if (particleSystemData.positionType == ParticleSystemData::PositionType::FREE)
                        position = particles[i].position;
                    else if (particleSystemData.positionType == ParticleSystemData::PositionType::PARENT)
                        position = Vector2(actor->getPosition()) + particles[i].position;

                    float size_2 = particles[i].size / 2.0F;
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

                vertexBuffer->setData(vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)));
            }
        }

        void ParticleSystem::emitParticles(uint32_t count)
        {
            if (particleCount + count > particleSystemData.maxParticles)
                count = particleSystemData.maxParticles - particleCount;

            if (count && actor)
            {
                Vector2 position;

                if (particleSystemData.positionType == ParticleSystemData::PositionType::FREE)
                    position = actor->convertLocalToWorld(Vector3());
                else if (particleSystemData.positionType == ParticleSystemData::PositionType::PARENT)
                    position = actor->convertLocalToWorld(Vector3()) - actor->getPosition();

                for (uint32_t i = particleCount; i < particleCount + count; ++i)
                {
                    if (particleSystemData.emitterType == ParticleSystemData::EmitterType::GRAVITY)
                    {
                        particles[i].life = fmaxf(particleSystemData.particleLifespan + particleSystemData.particleLifespanVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F);

                        particles[i].position = particleSystemData.sourcePosition + position + Vector2(particleSystemData.sourcePositionVariance.x * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine),
                                                                                                       particleSystemData.sourcePositionVariance.y * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));

                        particles[i].size = fmaxf(particleSystemData.startParticleSize + particleSystemData.startParticleSizeVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F);

                        float finishSize = fmaxf(particleSystemData.finishParticleSize + particleSystemData.finishParticleSizeVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F);
                        particles[i].deltaSize = (finishSize - particles[i].size) / particles[i].life;

                        particles[i].colorRed = clamp(particleSystemData.startColorRed + particleSystemData.startColorRedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);
                        particles[i].colorGreen = clamp(particleSystemData.startColorGreen + particleSystemData.startColorGreenVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);
                        particles[i].colorBlue = clamp(particleSystemData.startColorBlue + particleSystemData.startColorBlueVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);
                        particles[i].colorAlpha = clamp(particleSystemData.startColorAlpha + particleSystemData.startColorAlphaVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);

                        float finishColorRed = clamp(particleSystemData.finishColorRed + particleSystemData.finishColorRedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);
                        float finishColorGreen = clamp(particleSystemData.finishColorGreen + particleSystemData.finishColorGreenVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);
                        float finishColorBlue = clamp(particleSystemData.finishColorBlue + particleSystemData.finishColorBlueVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);
                        float finishColorAlpha = clamp(particleSystemData.finishColorAlpha + particleSystemData.finishColorAlphaVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F, 1.0F);

                        particles[i].deltaColorRed = (finishColorRed - particles[i].colorRed) / particles[i].life;
                        particles[i].deltaColorGreen = (finishColorGreen - particles[i].colorGreen) / particles[i].life;
                        particles[i].deltaColorBlue = (finishColorBlue - particles[i].colorBlue) / particles[i].life;
                        particles[i].deltaColorAlpha = (finishColorAlpha - particles[i].colorAlpha) / particles[i].life;

                        particles[i].rotation = particleSystemData.startRotation + particleSystemData.startRotationVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);

                        float finishRotation = particleSystemData.finishRotation + particleSystemData.finishRotationVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                        particles[i].deltaRotation = (finishRotation - particles[i].rotation) / particles[i].life;

                        particles[i].radialAcceleration = particleSystemData.radialAcceleration + particleSystemData.radialAcceleration * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                        particles[i].tangentialAcceleration = particleSystemData.tangentialAcceleration + particleSystemData.tangentialAcceleration * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);

                        if (particleSystemData.rotationIsDir)
                        {
                            float a = degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));
                            Vector2 v(cosf(a), sinf(a));
                            float s = particleSystemData.speed + particleSystemData.speedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                            Vector2 dir = v * s;
                            particles[i].direction = dir;
                            particles[i].rotation = -radToDeg(dir.getAngle());
                        }
                        else
                        {
                            float a = degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));
                            Vector2 v(cosf(a), sinf(a));
                            float s = particleSystemData.speed + particleSystemData.speedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                            Vector2 dir = v * s;
                            particles[i].direction = dir;
                        }
                    }
                    else
                    {
                        particles[i].radius = particleSystemData.maxRadius + particleSystemData.maxRadiusVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                        particles[i].angle = degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));
                        particles[i].degreesPerSecond = degToRad(particleSystemData.rotatePerSecond + particleSystemData.rotatePerSecondVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));

                        float endRadius = particleSystemData.minRadius + particleSystemData.minRadiusVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                        particles[i].deltaRadius = (endRadius - particles[i].radius) / particles[i].life;
                    }
                }

                particleCount += count;
            }
        }
    } // namespace scene
} // namespace ouzel
