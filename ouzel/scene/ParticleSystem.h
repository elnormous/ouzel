// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "scene/Component.h"
#include "utils/Types.h"
#include "scene/ParticleDefinition.h"
#include "math/Vector2.h"
#include "graphics/Color.h"
#include "graphics/Vertex.h"
#include "core/UpdateCallback.h"

namespace ouzel
{
    namespace scene
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

        class ParticleSystem: public Component
        {
        public:
            ParticleSystem();
            ParticleSystem(const std::string& filename);

            virtual void draw(const Matrix4& transformMatrix,
                              const graphics::Color& drawColor,
                              scene::Camera* camera) override;

            virtual void drawWireframe(const Matrix4& transformMatrix,
                                       const graphics::Color& drawColor,
                                       scene::Camera* camera) override;

            virtual void update(float delta);

            virtual bool initFromFile(const std::string& filename);

            void resume();
            void stop();
            void reset();

            bool isRunning() const { return running; }
            bool isActive() const { return active; }

            void setPositionType(ParticleDefinition::PositionType newPositionType) { positionType = newPositionType; }
            ParticleDefinition::PositionType getPositionType() const { return positionType; }

            void setFinishHandler(const std::function<void()>& handler) { finishHandler = handler; }

        protected:
            bool createParticleMesh();
            bool updateParticleMesh();

            void emitParticles(uint32_t count);

            ParticleDefinition particleDefinition;
            ParticleDefinition::PositionType positionType;

            graphics::ShaderPtr shader;
            graphics::BlendStatePtr blendState;
            graphics::TexturePtr texture;
            graphics::TexturePtr whitePixelTexture;

            std::vector<Particle> particles;

            graphics::MeshBufferPtr meshBuffer;
            graphics::IndexBufferPtr indexBuffer;
            graphics::VertexBufferPtr vertexBuffer;

            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPCT> vertices;

            uint32_t particleCount = 0;

            float emitCounter = 0.0f;
            float elapsed = 0.0f;
            bool active = false;
            bool running = false;
            bool finished = false;

            bool needsMeshUpdate = false;

            UpdateCallback updateCallback;

            std::function<void()> finishHandler;
        };
    } // namespace scene
} // namespace ouzel
