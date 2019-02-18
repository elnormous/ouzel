// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCENE_PARTICLESYSTEM_HPP
#define OUZEL_SCENE_PARTICLESYSTEM_HPP

#include <string>
#include <vector>
#include <functional>
#include "scene/Component.hpp"
#include "math/Vector2.hpp"
#include "math/Color.hpp"
#include "events/EventHandler.hpp"
#include "graphics/Vertex.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        struct ParticleSystemData final
        {
            enum class EmitterType
            {
                GRAVITY,
                RADIUS
            };

            enum class PositionType
            {
                FREE,
                PARENT, // relative to parent
                GROUPED
            };

            std::string name;

            uint32_t blendFuncSource = 0;
            uint32_t blendFuncDestination = 0;

            EmitterType emitterType = EmitterType::GRAVITY;
            uint32_t maxParticles = 0;
            float duration = 0.0F;
            float particleLifespan = 0.0F;
            float particleLifespanVariance = 0.0F;

            float speed = 0.0F;
            float speedVariance = 0.0F;

            Vector2F sourcePosition;
            Vector2F sourcePositionVariance;

            PositionType positionType = PositionType::FREE;

            float startParticleSize = 0.0F;
            float startParticleSizeVariance = 0.0F;

            float finishParticleSize = 0.0F;
            float finishParticleSizeVariance = 0.0F;

            float angle = 0.0F;
            float angleVariance = 0.0F;

            float startRotation = 0.0F;
            float startRotationVariance = 0.0F;

            float finishRotation = 0.0F;
            float finishRotationVariance = 0.0F;

            float rotatePerSecond = 0.0F;
            float rotatePerSecondVariance = 0.0F;

            float minRadius = 0.0F;
            float minRadiusVariance = 0.0F;

            float maxRadius = 0.0F;
            float maxRadiusVariance = 0.0F;

            float radialAcceleration = 0.0F;
            float radialAccelVariance = 0.0F;

            float tangentialAcceleration = 0.0F;
            float tangentialAccelVariance = 0.0F;

            bool absolutePosition = false;
            bool yCoordFlipped = false;
            bool rotationIsDir = false;

            Vector2F gravity = Vector2F(0.0F, 0.0F);

            float startColorRed = 0.0F;
            float startColorGreen = 0.0F;
            float startColorBlue = 0.0F;
            float startColorAlpha = 0.0F;

            float startColorRedVariance = 0.0F;
            float startColorGreenVariance = 0.0F;
            float startColorBlueVariance = 0.0F;
            float startColorAlphaVariance = 0.0F;

            float finishColorRed = 0.0F;
            float finishColorGreen = 0.0F;
            float finishColorBlue = 0.0F;
            float finishColorAlpha = 0.0F;

            float finishColorRedVariance = 0.0F;
            float finishColorGreenVariance = 0.0F;
            float finishColorBlueVariance = 0.0F;
            float finishColorAlphaVariance = 0.0F;

            float emissionRate = 0.0F;

            graphics::Texture* texture = nullptr;
        };

        class ParticleSystem: public Component
        {
        public:
            static constexpr uint32_t CLASS = Component::PARTICLE_SYSTEM;

            ParticleSystem();
            explicit ParticleSystem(const ParticleSystemData& initParticleSystemData);
            explicit ParticleSystem(const std::string& filename);

            void draw(const Matrix4F& transformMatrix,
                      float opacity,
                      const Matrix4F& renderViewProjection,
                      bool wireframe) override;

            void init(const ParticleSystemData& newParticleSystemData);
            void init(const std::string& filename);

            void resume();
            void stop();
            void reset();

            bool isRunning() const { return running; }
            bool isActive() const { return active; }

            inline ParticleSystemData::PositionType getPositionType() const { return particleSystemData.positionType; }
            inline void setPositionType(ParticleSystemData::PositionType newPositionType) { particleSystemData.positionType = newPositionType; }

        private:
            void update(float delta);
            bool handleUpdate(const UpdateEvent& event);

            void createParticleMesh();
            void updateParticleMesh();

            void emitParticles(uint32_t count);

            ParticleSystemData particleSystemData;

            graphics::Shader* shader = nullptr;
            graphics::BlendState* blendState = nullptr;
            graphics::Texture* texture = nullptr;
            graphics::Texture* whitePixelTexture = nullptr;

            struct Particle final
            {
                float life = 0.0F;

                Vector2F position;

                float colorRed = 0.0F;
                float colorGreen = 0.0F;
                float colorBlue = 0.0F;
                float colorAlpha = 0.0F;

                float deltaColorRed = 0.0F;
                float deltaColorGreen = 0.0F;
                float deltaColorBlue = 0.0F;
                float deltaColorAlpha = 0.0F;

                float angle = 0.0F;
                float speed = 0.0F;

                float size = 0.0F;
                float deltaSize = 0.0F;

                float rotation = 0.0F;
                float deltaRotation = 0.0F;

                float radialAcceleration = 0.0F;
                float tangentialAcceleration = 0.0F;

                Vector2F direction;
                float radius = 0.0F;
                float degreesPerSecond = 0.0F;
                float deltaRadius = 0.0F;
            };

            std::vector<Particle> particles;

            std::unique_ptr<graphics::Buffer> indexBuffer;
            std::unique_ptr<graphics::Buffer> vertexBuffer;

            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;

            uint32_t particleCount = 0;

            float emitCounter = 0.0F;
            float elapsed = 0.0F;
            float timeSinceUpdate = 0.0F;
            bool active = false;
            bool running = false;
            bool finished = false;

            bool needsMeshUpdate = false;

            EventHandler updateHandler;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCENE_PARTICLESYSTEM_HPP
