// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_SPRITE_HPP
#define OUZEL_SCENE_SPRITE_HPP

#include <list>
#include <map>
#include <memory>
#include <vector>
#include "Component.hpp"
#include "../events/EventHandler.hpp"
#include "../graphics/BlendState.hpp"
#include "../graphics/Buffer.hpp"
#include "../graphics/Material.hpp"
#include "../graphics/Shader.hpp"
#include "../graphics/Texture.hpp"
#include "../graphics/Vertex.hpp"
#include "../math/Box.hpp"
#include "../math/Rect.hpp"
#include "../math/Size.hpp"
#include "../math/Vector.hpp"

namespace ouzel::scene
{
    class SpriteData final
    {
    public:
        class Frame final
        {
        public:
            Frame(const std::string& frameName,
                  const math::Size<float, 2>& textureSize,
                  const math::Rect<float>& frameRectangle,
                  bool rotated,
                  const math::Size<float, 2>& sourceSize,
                  const math::Vector<float, 2>& sourceOffset,
                  const math::Vector<float, 2>& pivot);

            Frame(const std::string& frameName,
                  const std::vector<std::uint16_t>& indices,
                  const std::vector<graphics::Vertex>& vertices);

            Frame(const std::string& frameName,
                  const std::vector<std::uint16_t>& indices,
                  const std::vector<graphics::Vertex>& vertices,
                  const math::Rect<float>& frameRectangle,
                  const math::Size<float, 2>& sourceSize,
                  const math::Vector<float, 2>& sourceOffset,
                  const math::Vector<float, 2>& pivot);

            auto& getName() const noexcept { return name; }

            auto& getBoundingBox() const noexcept { return boundingBox; }
            auto getIndexCount() const noexcept { return indexCount; }
            auto& getIndexBuffer() const noexcept { return indexBuffer; }
            auto& getVertexBuffer() const noexcept { return vertexBuffer; }

        private:
            std::string name;
            math::Box<float, 2> boundingBox;
            std::uint32_t indexCount = 0;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;
        };

        struct Animation final
        {
            std::string name;
            std::vector<Frame> frames;
            float frameInterval = 0.1F;
        };

        std::map<std::string, Animation> animations;
        std::shared_ptr<graphics::Texture> texture;
        const graphics::BlendState* blendState = nullptr;
        const graphics::Shader* shader = nullptr;
    };

    class SpriteRenderer: public Component
    {
    public:
        SpriteRenderer();
        explicit SpriteRenderer(const SpriteData& spriteData);
        explicit SpriteRenderer(const std::string& filename);
        explicit SpriteRenderer(std::shared_ptr<graphics::Texture> texture,
                                std::uint32_t spritesX = 1, std::uint32_t spritesY = 1,
                                const math::Vector<float, 2>& pivot = math::Vector<float, 2>{0.5F, 0.5F});

        void init(const SpriteData& spriteData);
        void init(const std::string& filename);
        void init(std::shared_ptr<graphics::Texture> newTexture,
                  std::uint32_t spritesX = 1, std::uint32_t spritesY = 1,
                  const math::Vector<float, 2>& pivot = math::Vector<float, 2>{0.5F, 0.5F});

        void update(float delta);

        void draw(const math::Matrix<float, 4>& transformMatrix,
                  float opacity,
                  const math::Matrix<float, 4>& renderViewProjection,
                  bool wireframe) override;

        auto& getMaterial() const noexcept { return material; }
        void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

        auto& getOffset() const noexcept { return offset; }
        void setOffset(const math::Vector<float, 2>& newOffset);

        void play();
        void stop(bool resetAnimation = true);
        void reset();
        auto isPlaying() const noexcept { return playing; }

        auto& getAnimations() const noexcept { return animations; }
        auto getAnimation() const noexcept { return currentAnimation->animation; }
        auto& getAnimationName() const noexcept { return currentAnimation->animation->name; }
        bool hasAnimation(const std::string& animation) const;
        void setAnimation(const std::string& newAnimation, bool repeat = true);
        void addAnimation(const std::string& newAnimation, bool repeat = true);
        void setAnimationProgress(float progress);
        void setAnimationTime(float time);

    private:
        void updateBoundingBox();

        std::shared_ptr<graphics::Material> material;
        std::map<std::string, SpriteData::Animation> animations;

        struct QueuedAnimation final
        {
            const SpriteData::Animation* animation;
            bool repeat;
        };

        std::list<QueuedAnimation> animationQueue;
        std::list<QueuedAnimation>::const_iterator currentAnimation;

        math::Vector<float, 2> offset{};
        math::Matrix<float, 4> offsetMatrix = math::identityMatrix<float, 4>;

        bool playing = false;
        bool running = false;
        float currentTime = 0.0F;

        EventHandler updateHandler;
    };
}

#endif // OUZEL_SCENE_SPRITE_HPP
