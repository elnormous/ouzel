// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Drawable.h"
#include "Types.h"
#include "Size2.h"
#include "MeshBuffer.h"
#include "Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;

        class Sprite: public Drawable
        {
        public:
            static std::shared_ptr<Sprite> createFromFile(const std::string& filename, bool mipmaps = true);

            Sprite();
            virtual ~Sprite();

            virtual bool initFromFile(const std::string& filename, bool mipmaps = true);

            virtual void update(float delta);
            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& color) override;

            virtual void setOpacity(float opacity);

            virtual graphics::TexturePtr getTexture() const { return texture; }
            virtual void setTexture(const graphics::TexturePtr& newTexture);

            virtual graphics::ShaderPtr getShader() const { return shader; }
            virtual void setShader(const graphics::ShaderPtr& newShader);

            virtual const Size2& getSize() const { return size; }

            virtual const graphics::Color& getColor() const { return color; }
            virtual void setColor(const graphics::Color& newColor);

            virtual void play(bool pRepeat = true, float newFrameInterval = 0.1f);
            virtual void stop(bool resetAnimation = true);
            virtual void reset();
            virtual bool isPlaying() const { return playing; }

            const Vector2& getOffset() const { return offset; }
            void setOffset(const Vector2& newOffset);

        protected:
            bool loadSpriteSheet(const std::string& filename, bool mipmaps);
            void addFrame(const Rectangle& rectangle, const Size2& textureSize, bool rotated, const Size2& sourceSize, const Vector2& offset, const Vector2& pivot);

            void updateVertexColor();

            graphics::TexturePtr texture;
            graphics::ShaderPtr shader;
            graphics::BlendStatePtr blendState;

            float opacity = 1.0f;
            Size2 size;

            uint32_t frameCount = 0;
            std::vector<Rectangle> frameRectangles;
            std::vector<std::vector<graphics::VertexPCT>> frameVertices;
            std::vector<graphics::MeshBufferPtr> frameMeshBuffers;
            graphics::Color color = graphics::Color(255, 255, 255, 255);

            uint32_t currentFrame = 0;
            float frameInterval = 0.0f;
            bool playing = false;
            bool repeat = false;
            float timeSinceLastFrame = 0.0f;

            UpdateCallbackPtr updateCallback;

            Vector2 offset;
        };
    } // namespace scene
} // namespace ouzel
