// Ouzel by Elviss Strazdins

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include "ParticleSystem.hpp"
#include "SceneManager.hpp"
#include "Actor.hpp"
#include "Camera.hpp"
#include "Layer.hpp"
#include "../assets/Cache.hpp"
#include "../core/Engine.hpp"
#include "../math/Scalar.hpp"
#include "../storage/FileSystem.hpp"
#include "../utils/Utils.hpp"

namespace ouzel::scene
{
    namespace
    {
        constexpr float updateStep = 1.0F / 60.0F;
    }

    ParticleSystem::ParticleSystem():
        shader{engine->getCache().getShader(shaderTexture)},
        blendState{engine->getCache().getBlendState(blendAlpha)}
    {
        whitePixelTexture = engine->getCache().getTexture(textureWhitePixel);

        updateHandler.updateHandler = [this](const UpdateEvent& event) {
            update(event.delta);
            return false;
        };
    }

    ParticleSystem::ParticleSystem(const ParticleSystemData& initParticleSystemData):
        ParticleSystem{}
    {
        init(initParticleSystemData);
    }

    void ParticleSystem::draw(const math::Matrix<float, 4>& transformMatrix,
                              float opacity,
                              const math::Matrix<float, 4>& renderViewProjection,
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

            const math::Matrix<float, 4> transform =
                (particleSystemData.positionType == ParticleSystemData::PositionType::free ||
                 particleSystemData.positionType == ParticleSystemData::PositionType::parent) ?
                renderViewProjection :
                (particleSystemData.positionType == ParticleSystemData::PositionType::grouped) ?
                renderViewProjection * transformMatrix :
                throw std::runtime_error("Invalid position type");

            const auto colorVector = {1.0F, 1.0F, 1.0F, opacity};

            std::vector<std::vector<float>> pixelShaderConstants(1);
            pixelShaderConstants[0] = {std::begin(colorVector), std::end(colorVector)};

            std::vector<std::vector<float>> vertexShaderConstants(1);
            vertexShaderConstants[0] = {std::begin(transform.m), std::end(transform.m)};

            engine->getGraphics()->setPipelineState(blendState->getResource(),
                                                    shader->getResource(),
                                                    graphics::CullMode::none,
                                                    wireframe ? graphics::FillMode::wireframe : graphics::FillMode::solid);
            engine->getGraphics()->setShaderConstants(pixelShaderConstants,
                                                      vertexShaderConstants);
            engine->getGraphics()->setTextures({wireframe ? whitePixelTexture->getResource() : texture->getResource()});
            engine->getGraphics()->draw(indexBuffer->getResource(),
                                        particleCount * 6,
                                        sizeof(std::uint16_t),
                                        vertexBuffer->getResource(),
                                        graphics::DrawMode::triangleList,
                                        0);
        }
    }

    void ParticleSystem::update(float delta)
    {
        timeSinceUpdate += delta;

        bool needsBoundingBoxUpdate = false;

        while (timeSinceUpdate >= updateStep)
        {
            timeSinceUpdate -= updateStep;

            if (running && particleSystemData.emissionRate > 0.0F)
            {
                const float rate = 1.0F / particleSystemData.emissionRate;

                if (particleCount < particleSystemData.maxParticles)
                {
                    emitCounter += updateStep;
                    if (emitCounter < 0.0F)
                        emitCounter = 0.0F;
                }

                const auto emitCount = static_cast<std::uint32_t>(std::min(static_cast<float>(particleSystemData.maxParticles - particleCount), emitCounter / rate));
                emitParticles(emitCount);
                emitCounter -= rate * emitCount;

                elapsed += updateStep;
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
                finishEvent->type = Event::Type::animationFinish;
                finishEvent->component = this;
                engine->getEventDispatcher().dispatchEvent(std::move(finishEvent));

                return;
            }

            if (active)
            {
                for (std::uint32_t counter = particleCount; counter > 0; --counter)
                {
                    const std::size_t i = counter - 1;

                    particles[i].life -= updateStep;

                    if (particles[i].life >= 0.0F)
                    {
                        if (particleSystemData.emitterType == ParticleSystemData::EmitterType::gravity)
                        {
                            math::Vector<float, 2> radial{};

                            // radial acceleration
                            if (particles[i].position.v[0] == 0.0F || particles[i].position.v[1] == 0.0F)
                                radial = normalized(particles[i].position);

                            math::Vector<float, 2> tangential = radial;
                            radial *= particles[i].radialAcceleration;

                            // tangential acceleration
                            std::swap(tangential.v[0], tangential.v[1]);
                            tangential.v[0] *= - particles[i].tangentialAcceleration;
                            tangential.v[1] *= particles[i].tangentialAcceleration;

                            // (gravity + radial + tangential) * updateStep
                            math::Vector<float, 2> tmp{
                                radial.v[0] + tangential.v[0] + particleSystemData.gravity.v[0],
                                radial.v[1] + tangential.v[1] + particleSystemData.gravity.v[1]
                            };
                            tmp *= updateStep;

                            particles[i].direction.v[0] += tmp.v[0];
                            particles[i].direction.v[1] += tmp.v[1];
                            tmp.v[0] = particles[i].direction.v[0] * updateStep * (particleSystemData.yCoordFlipped ? 1.0F : 0.0F);
                            tmp.v[1] = particles[i].direction.v[1] * updateStep * (particleSystemData.yCoordFlipped ? 1.0F : 0.0F);
                            particles[i].position.v[0] += tmp.v[0];
                            particles[i].position.v[1] += tmp.v[1];
                        }
                        else
                        {
                            particles[i].angle += particles[i].degreesPerSecond * updateStep;
                            particles[i].radius += particles[i].deltaRadius * updateStep;
                            particles[i].position.v[0] = -std::cos(particles[i].angle) * particles[i].radius;
                            particles[i].position.v[1] = -std::sin(particles[i].angle) * particles[i].radius * (particleSystemData.yCoordFlipped ? 1.0F : 0.0F);
                        }

                        // color r,g,b,a
                        particles[i].colorRed += particles[i].deltaColorRed * updateStep;
                        particles[i].colorGreen += particles[i].deltaColorGreen * updateStep;
                        particles[i].colorBlue += particles[i].deltaColorBlue * updateStep;
                        particles[i].colorAlpha += particles[i].deltaColorAlpha * updateStep;

                        // size
                        particles[i].size += (particles[i].deltaSize * updateStep);
                        particles[i].size = std::max(0.0F, particles[i].size);

                        // angle
                        particles[i].rotation += particles[i].deltaRotation * updateStep;
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

            if (particleSystemData.positionType == ParticleSystemData::PositionType::free ||
                particleSystemData.positionType == ParticleSystemData::PositionType::parent)
            {
                if (actor)
                {
                    const auto& inverseTransform = actor->getInverseTransform();

                    for (std::uint32_t i = 0; i < particleCount; ++i)
                    {
                        auto position = math::Vector<float, 3>{particles[i].position};
                        inverseTransform.transformPoint(position);
                        boundingBox.insertPoint(position);
                    }
                }
            }
            else if (particleSystemData.positionType == ParticleSystemData::PositionType::grouped)
                for (std::uint32_t i = 0; i < particleCount; ++i)
            boundingBox.insertPoint(math::Vector<float, 3>{particles[i].position});
        }
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
                startEvent->type = Event::Type::animationStart;
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

        for (std::uint16_t i = 0; i < particleSystemData.maxParticles; ++i)
        {
            indices.push_back(i * 4 + 0);
            indices.push_back(i * 4 + 1);
            indices.push_back(i * 4 + 2);
            indices.push_back(i * 4 + 1);
            indices.push_back(i * 4 + 3);
            indices.push_back(i * 4 + 2);

            vertices.emplace_back(math::Vector<float, 3>{-1.0F, -1.0F, 0.0F}, math::whiteColor,
                                  math::Vector<float, 2>{0.0F, 1.0F}, math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(math::Vector<float, 3>{1.0F, -1.0F, 0.0F}, math::whiteColor,
                                  math::Vector<float, 2>{1.0F, 1.0F}, math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(math::Vector<float, 3>{-1.0F, 1.0F, 0.0F}, math::whiteColor,
                                  math::Vector<float, 2>{0.0F, 0.0F}, math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
            vertices.emplace_back(math::Vector<float, 3>{1.0F, 1.0F, 0.0F}, math::whiteColor,
                                  math::Vector<float, 2>{1.0F, 0.0F}, math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
        }

        indexBuffer = std::make_unique<graphics::Buffer>(*engine->getGraphics(),
                                                         graphics::BufferType::index,
                                                         graphics::Flags::none,
                                                         indices.data(),
                                                         static_cast<std::uint32_t>(getVectorSize(indices)));

        vertexBuffer = std::make_unique<graphics::Buffer>(*engine->getGraphics(),
                                                          graphics::BufferType::vertex,
                                                          graphics::Flags::dynamic,
                                                          vertices.data(),
                                                          static_cast<std::uint32_t>(getVectorSize(vertices)));

        particles.resize(particleSystemData.maxParticles);
    }

    void ParticleSystem::updateParticleMesh()
    {
        if (actor)
        {
            for (std::uint32_t counter = particleCount; counter > 0; --counter)
            {
                const std::size_t i = counter - 1;

                const math::Vector<float, 2> position = (particleSystemData.positionType == ParticleSystemData::PositionType::free) ?
                    particles[i].position :
                    (particleSystemData.positionType == ParticleSystemData::PositionType::parent) ?
                    math::Vector<float, 2>{actor->getPosition()} + particles[i].position :
                    (particleSystemData.positionType == ParticleSystemData::PositionType::grouped) ?
                    math::Vector<float, 2>{} :
                    throw std::runtime_error("Invalid position type");

                const float halfSize = particles[i].size / 2.0F;
                const math::Vector<float, 2> v1{-halfSize, -halfSize};
                const math::Vector<float, 2> v2{halfSize, halfSize};

                const float r = -math::degToRad(particles[i].rotation);
                const float cr = std::cos(r);
                const float sr = std::sin(r);

                const math::Vector<float, 2> a{v1.v[0] * cr - v1.v[1] * sr, v1.v[0] * sr + v1.v[1] * cr};
                const math::Vector<float, 2> b{v2.v[0] * cr - v1.v[1] * sr, v2.v[0] * sr + v1.v[1] * cr};
                const math::Vector<float, 2> c{v2.v[0] * cr - v2.v[1] * sr, v2.v[0] * sr + v2.v[1] * cr};
                const math::Vector<float, 2> d{v1.v[0] * cr - v2.v[1] * sr, v1.v[0] * sr + v2.v[1] * cr};

                const math::Color color{
                    particles[i].colorRed,
                    particles[i].colorGreen,
                    particles[i].colorBlue,
                    particles[i].colorAlpha
                };

                vertices[i * 4 + 0].position = math::Vector<float, 3>{a + position};
                vertices[i * 4 + 0].color = color;

                vertices[i * 4 + 1].position = math::Vector<float, 3>{b + position};
                vertices[i * 4 + 1].color = color;

                vertices[i * 4 + 2].position = math::Vector<float, 3>{d + position};
                vertices[i * 4 + 2].color = color;

                vertices[i * 4 + 3].position = math::Vector<float, 3>{c + position};
                vertices[i * 4 + 3].color = color;
            }

            vertexBuffer->setData(vertices.data(), static_cast<std::uint32_t>(getVectorSize(vertices)));
        }
    }

    void ParticleSystem::emitParticles(std::uint32_t count)
    {
        if (particleCount + count > particleSystemData.maxParticles)
            count = particleSystemData.maxParticles - particleCount;

        if (count && actor)
        {
            const math::Vector<float, 2> position = (particleSystemData.positionType == ParticleSystemData::PositionType::free) ?
                math::Vector<float, 2>{actor->convertLocalToWorld(math::Vector<float, 3>{})} :
                (particleSystemData.positionType == ParticleSystemData::PositionType::parent) ?
                math::Vector<float, 2>{actor->convertLocalToWorld(math::Vector<float, 3>{}) - actor->getPosition()} :
                (particleSystemData.positionType == ParticleSystemData::PositionType::grouped) ?
                math::Vector<float, 2>{} :
                throw std::runtime_error("Invalid position type");

            for (std::uint32_t i = particleCount; i < particleCount + count; ++i)
            {
                if (particleSystemData.emitterType == ParticleSystemData::EmitterType::gravity)
                {
                    particles[i].life = std::max(particleSystemData.particleLifespan + particleSystemData.particleLifespanVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F);

                    particles[i].position = particleSystemData.sourcePosition + position + math::Vector<float, 2>{
                        particleSystemData.sourcePositionVariance.v[0] * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine),
                        particleSystemData.sourcePositionVariance.v[1] * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine)
                    };

                    particles[i].size = std::max(particleSystemData.startParticleSize + particleSystemData.startParticleSizeVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F);

                    const float finishSize = std::max(particleSystemData.finishParticleSize + particleSystemData.finishParticleSizeVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F);
                    particles[i].deltaSize = (finishSize - particles[i].size) / particles[i].life;

                    particles[i].colorRed = std::clamp(particleSystemData.startColorRed + particleSystemData.startColorRedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);
                    particles[i].colorGreen = std::clamp(particleSystemData.startColorGreen + particleSystemData.startColorGreenVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);
                    particles[i].colorBlue = std::clamp(particleSystemData.startColorBlue + particleSystemData.startColorBlueVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);
                    particles[i].colorAlpha = std::clamp(particleSystemData.startColorAlpha + particleSystemData.startColorAlphaVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);

                    const float finishColorRed = std::clamp(particleSystemData.finishColorRed + particleSystemData.finishColorRedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);
                    const float finishColorGreen = std::clamp(particleSystemData.finishColorGreen + particleSystemData.finishColorGreenVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);
                    const float finishColorBlue = std::clamp(particleSystemData.finishColorBlue + particleSystemData.finishColorBlueVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);
                    const float finishColorAlpha = std::clamp(particleSystemData.finishColorAlpha + particleSystemData.finishColorAlphaVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine), 0.0F, 1.0F);

                    particles[i].deltaColorRed = (finishColorRed - particles[i].colorRed) / particles[i].life;
                    particles[i].deltaColorGreen = (finishColorGreen - particles[i].colorGreen) / particles[i].life;
                    particles[i].deltaColorBlue = (finishColorBlue - particles[i].colorBlue) / particles[i].life;
                    particles[i].deltaColorAlpha = (finishColorAlpha - particles[i].colorAlpha) / particles[i].life;

                    particles[i].rotation = particleSystemData.startRotation + particleSystemData.startRotationVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);

                    const float finishRotation = particleSystemData.finishRotation + particleSystemData.finishRotationVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);
                    particles[i].deltaRotation = (finishRotation - particles[i].rotation) / particles[i].life;

                    particles[i].radialAcceleration = particleSystemData.radialAcceleration + particleSystemData.radialAcceleration * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);
                    particles[i].tangentialAcceleration = particleSystemData.tangentialAcceleration + particleSystemData.tangentialAcceleration * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);

                    if (particleSystemData.rotationIsDir)
                    {
                        const float a = math::degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine));
                        const math::Vector<float, 2> v{std::cos(a), std::sin(a)};
                        const float s = particleSystemData.speed + particleSystemData.speedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);
                        const auto dir = v * s;
                        particles[i].direction = dir;
                        particles[i].rotation = -math::radToDeg(getAngle(dir));
                    }
                    else
                    {
                        const float a = math::degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine));
                        const math::Vector<float, 2> v{std::cos(a), std::sin(a)};
                        const float s = particleSystemData.speed + particleSystemData.speedVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);
                        const auto dir = v * s;
                        particles[i].direction = dir;
                    }
                }
                else
                {
                    particles[i].radius = particleSystemData.maxRadius + particleSystemData.maxRadiusVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);
                    particles[i].angle = math::degToRad(particleSystemData.angle + particleSystemData.angleVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine));
                    particles[i].degreesPerSecond = math::degToRad(particleSystemData.rotatePerSecond + particleSystemData.rotatePerSecondVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine));

                    const float endRadius = particleSystemData.minRadius + particleSystemData.minRadiusVariance * std::uniform_real_distribution<float>{-1.0F, 1.0F}(core::randomEngine);
                    particles[i].deltaRadius = (endRadius - particles[i].radius) / particles[i].life;
                }
            }

            particleCount += count;
        }
    }
}
