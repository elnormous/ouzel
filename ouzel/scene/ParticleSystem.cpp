// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include "ParticleSystem.hpp"
#include "core/Engine.hpp"
#include "SceneManager.hpp"
#include "assets/Cache.hpp"
#include "storage/FileSystem.hpp"
#include "Actor.hpp"
#include "Camera.hpp"
#include "Layer.hpp"
#include "utils/Utils.hpp"
#include "math/MathUtils.hpp"

static constexpr float UPDATE_STEP = 1.0F / 60.0F;

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
        
        void ParticleSystem::draw(const Matrix4F& transformMatrix,
                                  float opacity,
                                  const Matrix4F& renderViewProjection,
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

                const Matrix4F transform =
                    (particleSystemData.positionType == ParticleSystemData::PositionType::FREE ||
                     particleSystemData.positionType == ParticleSystemData::PositionType::PARENT) ?
                    renderViewProjection :
                    (particleSystemData.positionType == ParticleSystemData::PositionType::GROUPED) ?
                    renderViewProjection * transformMatrix : Matrix4F::identity();

                const float colorVector[] = {1.0F, 1.0F, 1.0F, opacity};

                std::vector<std::vector<float>> pixelShaderConstants(1);
                pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

                std::vector<std::vector<float>> vertexShaderConstants(1);
                vertexShaderConstants[0] = {std::begin(transform.m), std::end(transform.m)};

                engine->getRenderer()->setPipelineState(blendState->getResource(),
                                                        shader->getResource(),
                                                        graphics::CullMode::None,
                                                        wireframe ? graphics::FillMode::Wireframe : graphics::FillMode::Solid);
                engine->getRenderer()->setShaderConstants(pixelShaderConstants,
                                                          vertexShaderConstants);
                engine->getRenderer()->setTextures({wireframe ? whitePixelTexture->getResource() : texture->getResource()});
                engine->getRenderer()->draw(indexBuffer->getResource(),
                                            particleCount * 6,
                                            sizeof(uint16_t),
                                            vertexBuffer->getResource(),
                                            graphics::DrawMode::TriangleList,
                                            0);
            }
        }

        void ParticleSystem::update(float delta)
        {
            timeSinceUpdate += delta;

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

                    auto emitCount = static_cast<uint32_t>(std::min(static_cast<float>(particleSystemData.maxParticles - particleCount), emitCounter / rate));
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

                    auto finishEvent = std::make_unique<AnimationEvent>();
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
                                Vector2F tmp;
                                Vector2F radial;
                                Vector2F tangential;

                                // radial acceleration
                                if (particles[i].position.v[0] == 0.0F || particles[i].position.v[1] == 0.0F)
                                {
                                    radial = particles[i].position;
                                    radial.normalize();
                                }
                                tangential = radial;
                                radial *= particles[i].radialAcceleration;

                                // tangential acceleration
                                std::swap(tangential.v[0], tangential.v[1]);
                                tangential.v[0] *= - particles[i].tangentialAcceleration;
                                tangential.v[1] *= particles[i].tangentialAcceleration;

                                // (gravity + radial + tangential) * UPDATE_STEP
                                tmp.v[0] = radial.v[0] + tangential.v[0] + particleSystemData.gravity.v[0];
                                tmp.v[1] = radial.v[1] + tangential.v[1] + particleSystemData.gravity.v[1];
                                tmp.v[0] *= UPDATE_STEP;
                                tmp.v[1] *= UPDATE_STEP;

                                particles[i].direction.v[0] += tmp.v[0];
                                particles[i].direction.v[1] += tmp.v[1];
                                tmp.v[0] = particles[i].direction.v[0] * UPDATE_STEP * (particleSystemData.yCoordFlipped ? 1.0F : 0.0F);
                                tmp.v[1] = particles[i].direction.v[1] * UPDATE_STEP * (particleSystemData.yCoordFlipped ? 1.0F : 0.0F);
                                particles[i].position.v[0] += tmp.v[0];
                                particles[i].position.v[1] += tmp.v[1];
                            }
                            else
                            {
                                particles[i].angle += particles[i].degreesPerSecond * UPDATE_STEP;
                                particles[i].radius += particles[i].deltaRadius * UPDATE_STEP;
                                particles[i].position.v[0] = -cos(particles[i].angle) * particles[i].radius;
                                particles[i].position.v[1] = -sin(particles[i].angle) * particles[i].radius * (particleSystemData.yCoordFlipped ? 1.0F : 0.0F);
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
                        const Matrix4F& inverseTransform = actor->getInverseTransform();

                        for (uint32_t i = 0; i < particleCount; ++i)
                        {
                            auto position = Vector3F(particles[i].position);
                            inverseTransform.transformPoint(position);
                            boundingBox.insertPoint(position);
                        }
                    }
                }
                else if (particleSystemData.positionType == ParticleSystemData::PositionType::GROUPED)
                {
                    for (uint32_t i = 0; i < particleCount; ++i)
                        boundingBox.insertPoint(Vector3F(particles[i].position));
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

        void ParticleSystem::resume()
        {
            if (!running)
            {
                finished = false;
                running = true;

                if (!active)
                {
                    active = true;
                    engine->getEventDispatcher().addEventHandler(updateHandler);
                }

                if (particleCount == 0)
                {
                    auto startEvent = std::make_unique<AnimationEvent>();
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

                vertices.emplace_back(Vector3F{-1.0F, -1.0F, 0.0F}, Color::white(),
                                      Vector2F{0.0F, 1.0F}, Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{1.0F, -1.0F, 0.0F}, Color::white(),
                                      Vector2F{1.0F, 1.0F}, Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{-1.0F, 1.0F, 0.0F}, Color::white(),
                                      Vector2F{0.0F, 0.0F}, Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{1.0F, 1.0F, 0.0F}, Color::white(),
                                      Vector2F{1.0F, 0.0F}, Vector3F{0.0F, 0.0F, -1.0F});
            }

            indexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                             graphics::BufferType::Index, 0,
                                                             indices.data(),
                                                             static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                              graphics::BufferType::Vertex,
                                                              graphics::Flags::DYNAMIC,
                                                              vertices.data(),
                                                              static_cast<uint32_t>(getVectorSize(vertices)));

            particles.resize(particleSystemData.maxParticles);
        }

        void ParticleSystem::updateParticleMesh()
        {
            if (actor)
            {
                for (uint32_t counter = particleCount; counter > 0; --counter)
                {
                    size_t i = counter - 1;

                    Vector2F position;

                    if (particleSystemData.positionType == ParticleSystemData::PositionType::FREE)
                        position = particles[i].position;
                    else if (particleSystemData.positionType == ParticleSystemData::PositionType::PARENT)
                        position = Vector2F(actor->getPosition()) + particles[i].position;

                    float size_2 = particles[i].size / 2.0F;
                    Vector2F v1(-size_2, -size_2);
                    Vector2F v2(size_2, size_2);

                    float r = -degToRad(particles[i].rotation);
                    float cr = cos(r);
                    float sr = sin(r);

                    Vector2F a(v1.v[0] * cr - v1.v[1] * sr, v1.v[0] * sr + v1.v[1] * cr);
                    Vector2F b(v2.v[0] * cr - v1.v[1] * sr, v2.v[0] * sr + v1.v[1] * cr);
                    Vector2F c(v2.v[0] * cr - v2.v[1] * sr, v2.v[0] * sr + v2.v[1] * cr);
                    Vector2F d(v1.v[0] * cr - v2.v[1] * sr, v1.v[0] * sr + v2.v[1] * cr);

                    Color color(static_cast<uint8_t>(particles[i].colorRed * 255),
                                static_cast<uint8_t>(particles[i].colorGreen * 255),
                                static_cast<uint8_t>(particles[i].colorBlue * 255),
                                static_cast<uint8_t>(particles[i].colorAlpha * 255));

                    vertices[i * 4 + 0].position = Vector3F(a + position);
                    vertices[i * 4 + 0].color = color;

                    vertices[i * 4 + 1].position = Vector3F(b + position);
                    vertices[i * 4 + 1].color = color;

                    vertices[i * 4 + 2].position = Vector3F(d + position);
                    vertices[i * 4 + 2].color = color;

                    vertices[i * 4 + 3].position = Vector3F(c + position);
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
                Vector2F position;

                if (particleSystemData.positionType == ParticleSystemData::PositionType::FREE)
                    position = Vector2F(actor->convertLocalToWorld(Vector3F()));
                else if (particleSystemData.positionType == ParticleSystemData::PositionType::PARENT)
                    position = Vector2F(actor->convertLocalToWorld(Vector3F()) - actor->getPosition());

                for (uint32_t i = particleCount; i < particleCount + count; ++i)
                {
                    if (particleSystemData.emitterType == ParticleSystemData::EmitterType::GRAVITY)
                    {
                        particles[i].life = std::max(particleSystemData.particleLifespan + particleSystemData.particleLifespanVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F);

                        particles[i].position = particleSystemData.sourcePosition + position + Vector2F(particleSystemData.sourcePositionVariance.v[0] * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine),
                                                                                                        particleSystemData.sourcePositionVariance.v[1] * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));

                        particles[i].size = std::max(particleSystemData.startParticleSize + particleSystemData.startParticleSizeVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F);

                        float finishSize = std::max(particleSystemData.finishParticleSize + particleSystemData.finishParticleSizeVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine), 0.0F);
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
                            Vector2F v(cos(a), sin(a));
                            float s = particleSystemData.speed + particleSystemData.speedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                            Vector2F dir = v * s;
                            particles[i].direction = dir;
                            particles[i].rotation = -radToDeg(dir.getAngle());
                        }
                        else
                        {
                            float a = degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine));
                            Vector2F v(cos(a), sin(a));
                            float s = particleSystemData.speed + particleSystemData.speedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(randomEngine);
                            Vector2F dir = v * s;
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
