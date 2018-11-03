// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <list>
#include "scene/Component.hpp"
#include "math/Size2.hpp"
#include "math/Vector2.hpp"
#include "scene/SpriteData.hpp"
#include "events/EventHandler.hpp"
#include "graphics/Material.hpp"

namespace ouzel
{
    namespace scene
    {
        class Sprite: public Component
        {
        public:
            static const uint32_t CLASS = Component::SPRITE;

            Sprite();
            explicit Sprite(const SpriteData& spriteData);
            explicit Sprite(const std::string& filename);
            explicit Sprite(std::shared_ptr<graphics::Texture> texture,
                            uint32_t spritesX = 1, uint32_t spritesY = 1,
                            const Vector2& pivot = Vector2(0.5F, 0.5F));

            void init(const SpriteData& spriteData);
            void init(const std::string& filename);
            void init(std::shared_ptr<graphics::Texture> newTexture,
                      uint32_t spritesX = 1, uint32_t spritesY = 1,
                      const Vector2& pivot = Vector2(0.5F, 0.5F));

            void update(float delta);

            void draw(const Matrix4& transformMatrix,
                      float opacity,
                      const Matrix4& renderViewProjection,
                      bool wireframe) override;

            inline const std::shared_ptr<graphics::Material>& getMaterial() const { return material; }
            inline void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

            inline const Vector2& getOffset() const { return offset; }
            void setOffset(const Vector2& newOffset);

            void play();
            void stop(bool resetAnimation = true);
            void reset();
            inline bool isPlaying() const { return playing; }

            inline const std::map<std::string, SpriteData::Animation>& getAnimations() const { return animations; }
            inline const SpriteData::Animation* getAnimation() const { return currentAnimation->animation; }
            inline std::string getAnimationName() const { return currentAnimation->animation->name; }
            bool hasAnimation(const std::string& animation) const;
            void setAnimation(const std::string& newAnimation, bool repeat = true);
            void addAnimation(const std::string& newAnimation, bool repeat = true);
            void setAnimationProgress(float progress);
            void setAnimationTime(float time);

        private:
            bool handleUpdate(const UpdateEvent& event);
            void updateBoundingBox();

            std::shared_ptr<graphics::Material> material;
            std::map<std::string, SpriteData::Animation> animations;

            struct QueuedAnimation
            {
                const SpriteData::Animation* animation;
                bool repeat;
            };

            std::list<QueuedAnimation> animationQueue;
            std::list<QueuedAnimation>::iterator currentAnimation;

            std::shared_ptr<graphics::Texture> whitePixelTexture;

            Vector2 offset;
            Matrix4 offsetMatrix = Matrix4::identity();

            bool playing = false;
            bool running = false;
            float currentTime = 0.0F;

            EventHandler updateHandler;
        };
    } // namespace scene
} // namespace ouzel
