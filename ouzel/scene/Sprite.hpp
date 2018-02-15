// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.hpp"
#include "math/Size2.hpp"
#include "math/Vector2.hpp"
#include "scene/SpriteData.hpp"
#include "core/UpdateCallback.hpp"
#include "graphics/Material.hpp"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class Sprite: public Component
        {
        public:
            static const uint32_t TYPE = Component::SPRITE;

            Sprite();
            Sprite(const SpriteData& spriteData);
            Sprite(const std::string& filename, bool mipmaps = true,
                   uint32_t spritesX = 1, uint32_t spritesY = 1,
                   const Vector2& pivot = Vector2(0.5f, 0.5f));
            Sprite(std::shared_ptr<graphics::Texture> texture,
                   uint32_t spritesX = 1, uint32_t spritesY = 1,
                   const Vector2& pivot = Vector2(0.5f, 0.5f));

            bool init(const SpriteData& spriteData);
            bool init(const std::string& filename, bool mipmaps = true,
                      uint32_t spritesX = 1, uint32_t spritesY = 1,
                      const Vector2& pivot = Vector2(0.5f, 0.5f));
            bool init(std::shared_ptr<graphics::Texture> newTexture,
                      uint32_t spritesX = 1, uint32_t spritesY = 1,
                      const Vector2& pivot = Vector2(0.5f, 0.5f));

            virtual void update(float delta);

            virtual void draw(const Matrix4& transformMatrix,
                              float opacity,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rect& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rect& scissorRectangle) override;

            inline const std::shared_ptr<graphics::Material>& getMaterial() const { return material; }
            inline void setMaterial(const std::shared_ptr<graphics::Material>& newMaterial) { material = newMaterial; }

            virtual const Size2& getSize() const { return size; }

            inline const Vector2& getOffset() const { return offset; }
            void setOffset(const Vector2& newOffset);

            virtual void play(bool repeat = true, float newFrameInterval = 0.1f);
            virtual void stop(bool resetAnimation = true);
            virtual void reset();
            virtual bool isPlaying() const { return playing; }

            inline const std::vector<SpriteFrame>& getFrames() const { return frames; }
            virtual void setCurrentFrame(uint32_t frame);

        protected:
            void updateBoundingBox();

            std::shared_ptr<graphics::Material> material;
            std::vector<SpriteFrame> frames;

            std::shared_ptr<graphics::Texture> whitePixelTexture;

            Size2 size;
            Vector2 offset;
            Matrix4 offsetMatrix = Matrix4::IDENTITY;

            uint32_t currentFrame = 0;
            float frameInterval = 0.0f;
            bool playing = false;
            bool repeating = false;
            float timeSinceLastFrame = 0.0f;

            UpdateCallback updateCallback;
        };
    } // namespace scene
} // namespace ouzel
