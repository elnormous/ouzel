// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "scene/Component.h"
#include "utils/Types.h"
#include "math/Size2.h"
#include "scene/SpriteFrame.h"
#include "core/UpdateCallback.h"
#include "graphics/BlendState.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class Sprite: public Component
        {
        public:
            Sprite();
            Sprite(const std::vector<SpriteFrame>& spriteFrames);
            Sprite(const std::string& filename, bool mipmaps = true);

            bool initFromSpriteFrames(const std::vector<SpriteFrame>& spriteFrames);
            bool initFromFile(const std::string& filename, bool mipmaps = true);

            virtual void update(float delta);

            virtual void draw(const Matrix4& transformMatrix,
                              const Color& drawColor,
                              scene::Camera* camera) override;

            virtual void drawWireframe(const Matrix4& transformMatrix,
                                       const Color& drawColor,
                                       scene::Camera* camera) override;

            virtual const std::shared_ptr<graphics::Shader>& getShader() const { return shader; }
            virtual void setShader(const std::shared_ptr<graphics::Shader>& newShader) { shader = newShader; }

            virtual const std::shared_ptr<graphics::BlendState>& getBlendState() const { return blendState; }
            virtual void setBlendState(const std::shared_ptr<graphics::BlendState>& newBlendState)  { blendState = newBlendState; }

            virtual const Size2& getSize() const { return size; }

            const Vector2& getOffset() const { return offset; }
            void setOffset(const Vector2& newOffset);

            virtual void play(bool repeat = true, float newFrameInterval = 0.1f);
            virtual void stop(bool resetAnimation = true);
            virtual void reset();
            virtual bool isPlaying() const { return playing; }

            const std::vector<SpriteFrame>& getFrames() const { return frames; }
            virtual void setCurrentFrame(uint32_t frame);

        protected:
            void updateBoundingBox();

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;
            std::shared_ptr<graphics::Texture> whitePixelTexture;

            Size2 size;
            Vector2 offset;
            Matrix4 offsetMatrix = Matrix4::IDENTITY;

            std::vector<SpriteFrame> frames;

            uint32_t currentFrame = 0;
            float frameInterval = 0.0f;
            bool playing = false;
            bool repeating = false;
            float timeSinceLastFrame = 0.0f;

            UpdateCallback updateCallback;
        };
    } // namespace scene
} // namespace ouzel
