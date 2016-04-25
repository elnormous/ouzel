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
            virtual void draw(const Matrix4& projection, const Matrix4& transform) override;

            virtual void setOpacity(float opacity);

            virtual graphics::TexturePtr getTexture() const { return _texture; }
            virtual void setTexture(const graphics::TexturePtr& texture);

            virtual graphics::ShaderPtr getShader() const { return _shader; }
            virtual void setShader(const graphics::ShaderPtr& shader);

            virtual const Size2& getSize() const { return _size; }

            virtual const graphics::Color& getColor() const { return _color; }
            virtual void setColor(const graphics::Color& color);

            virtual void play(bool repeat = true, float frameInterval = 0.1f);
            virtual void stop(bool resetAnimation = true);
            virtual void reset();
            virtual bool isPlaying() const { return _playing; }

            const Vector2& getOffset() const { return _offset; }
            void setOffset(const Vector2& offset);

        protected:
            bool loadSpriteSheet(const std::string& filename, bool mipmaps);
            void addFrame(const Rectangle& rectangle, const Size2& textureSize, bool rotated, const Size2& sourceSize, const Vector2& offset, const Vector2& pivot);

            void updateVertexColor();

            graphics::TexturePtr _texture;
            graphics::ShaderPtr _shader;
            graphics::BlendStatePtr _blendState;

            float _opacity = 1.0f;
            Size2 _size;

            uint32_t _frameCount = 0;
            std::vector<Rectangle> _frameRectangles;
            std::vector<std::vector<graphics::VertexPCT>> _frameVertices;
            std::vector<graphics::MeshBufferPtr> _frameMeshBuffers;
            graphics::Color _color = graphics::Color(255, 255, 255, 255);

            uint32_t _currentFrame = 0;
            float _frameInterval = 0.0f;
            bool _playing = false;
            bool _repeat = false;
            float _timeSinceLastFrame = 0.0f;

            UpdateCallbackPtr _updateCallback;

            Vector2 _offset;
        };
    } // namespace scene
} // namespace ouzel
