// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_PARTICLESYSTEM_HPP
#define OUZEL_SCENE_PARTICLESYSTEM_HPP

#include <string>
#include <vector>
#include <functional>
#include "Component.hpp"
#include "../events/EventHandler.hpp"
#include "../graphics/Vertex.hpp"
#include "../graphics/BlendState.hpp"
#include "../graphics/Buffer.hpp"
#include "../graphics/Shader.hpp"
#include "../graphics/Texture.hpp"
#include "../math/Color.hpp"
#include "../math/Vector.hpp"

namespace ouzel::scene
{
    struct ParticleSystemData final
    {
        enum class EmitterType
        {
            gravity,
            radius
        };

        enum class PositionType
        {
            free,
            parent, // relative to parent
            grouped
        };

        std::string name;

        std::uint32_t blendFuncSource = 0;
        std::uint32_t blendFuncDestination = 0;

        EmitterType emitterType = EmitterType::gravity;
        std::uint32_t maxParticles = 0;
        float duration = 0.0F;
        float particleLifespan = 0.0F;
        float particleLifespanVariance = 0.0F;

        float speed = 0.0F;
        float speedVariance = 0.0F;

        math::Vector<float, 2> sourcePosition{};
        math::Vector<float, 2> sourcePositionVariance{};

        PositionType positionType = PositionType::free;

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

        math::Vector<float, 2> gravity{};

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

        std::shared_ptr<graphics::Texture> texture;
    };

    class ParticleSystem final: public Component
    {
    public:
        ParticleSystem();
        explicit ParticleSystem(const ParticleSystemData& initParticleSystemData);

        void draw(const math::Matrix<float, 4>& transformMatrix,
                  const float opacity,
                  const math::Matrix<float, 4>& renderViewProjection,
                  const bool wireframe) override;

        void init(const ParticleSystemData& newParticleSystemData);

        void resume();
        void stop();
        void reset();

        auto isRunning() const noexcept { return running; }
        auto isActive() const noexcept { return active; }

        auto getPositionType() const noexcept
        {
            return particleSystemData.positionType;
        }

        void setPositionType(const ParticleSystemData::PositionType newPositionType) noexcept
        {
            particleSystemData.positionType = newPositionType;
        }

    private:
        void update(const float delta);

        void createParticleMesh();
        void updateParticleMesh();

        void emitParticles(const std::size_t count);

        ParticleSystemData particleSystemData;

        const graphics::Shader* shader = nullptr;
        const graphics::BlendState* blendState = nullptr;
        std::shared_ptr<graphics::Texture> texture;
        std::shared_ptr<graphics::Texture> whitePixelTexture;

        struct Particle final
        {
            float life = 0.0F;

            math::Vector<float, 2> position{};

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

            math::Vector<float, 2> direction{};
            float radius = 0.0F;
            float degreesPerSecond = 0.0F;
            float deltaRadius = 0.0F;
        };

        std::vector<Particle> particles;

        std::unique_ptr<graphics::Buffer> indexBuffer;
        std::unique_ptr<graphics::Buffer> vertexBuffer;

        std::vector<std::uint16_t> indices;
        std::vector<graphics::Vertex> vertices;

        std::size_t particleCount = 0;

        float emitCounter = 0.0F;
        float elapsed = 0.0F;
        float timeSinceUpdate = 0.0F;
        bool active = false;
        bool running = false;
        bool finished = false;

        bool needsMeshUpdate = false;

        EventHandler updateHandler;
    };
}

#endif // OUZEL_SCENE_PARTICLESYSTEM_HPP
