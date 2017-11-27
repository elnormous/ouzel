// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <functional>
#include "scene/Component.hpp"
#include "scene/ParticleSystemData.hpp"
#include "math/Vector2.hpp"
#include "math/Color.hpp"
#include "graphics/Vertex.hpp"
#include "core/UpdateCallback.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
    namespace scene
    {
        class ParticleSystem: public Component
        {
        public:
            static const uint32_t TYPE = Component::PARTICLE_SYSTEM;

            ParticleSystem();
            ParticleSystem(const std::string& filename);

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rectangle& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rectangle& scissorRectangle) override;

            bool init(const ParticleSystemData& newParticleSystemData);
            bool init(const std::string& filename);

            void resume();
            void stop();
            void reset();

            bool isRunning() const { return running; }
            bool isActive() const { return active; }

            void setPositionType(ParticleSystemData::PositionType newPositionType) { particleSystemData.positionType = newPositionType; }
            ParticleSystemData::PositionType getPositionType() const { return particleSystemData.positionType; }

        protected:
            void update(float delta);

            bool createParticleMesh();
            bool updateParticleMesh();

            void emitParticles(uint32_t count);

            ParticleSystemData particleSystemData;

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;
            std::shared_ptr<graphics::Texture> texture;
            std::shared_ptr<graphics::Texture> whitePixelTexture;

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

            std::vector<Particle> particles;

            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;

            std::vector<uint16_t> indices;
            std::vector<graphics::Vertex> vertices;

            uint32_t particleCount = 0;

            float emitCounter = 0.0f;
            float elapsed = 0.0f;
            float timeSinceUpdate = 0.0f;
            bool active = false;
            bool running = false;
            bool finished = false;

            bool needsMeshUpdate = false;

            UpdateCallback updateCallback;
        };
    } // namespace scene
} // namespace ouzel
