// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <vector>
#include "math/Box2.hpp"
#include "math/Rect.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/MeshBuffer.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Vertex.hpp"

namespace ouzel
{
    namespace scene
    {
        class SpriteData
        {
        public:
            class Frame
            {
            public:
                Frame(const std::string& frameName,
                      const Size2& textureSize,
                      const Rect& frameRectangle,
                      bool rotated,
                      const Size2& sourceSize,
                      const Vector2& sourceOffset,
                      const Vector2& pivot);

                Frame(const std::string& frameName,
                      const std::vector<uint16_t>& indices,
                      const std::vector<graphics::Vertex>& vertices);

                Frame(const std::string& frameName,
                      const std::vector<uint16_t>& indices,
                      const std::vector<graphics::Vertex>& vertices,
                      const Rect& frameRectangle,
                      const Size2& sourceSize,
                      const Vector2& sourceOffset,
                      const Vector2& pivot);

                inline const std::string& getName() const { return name; }

                inline const Box2& getBoundingBox() const { return boundingBox; }
                inline const std::shared_ptr<graphics::MeshBuffer>& getMeshBuffer() const { return meshBuffer; }

            protected:
                std::string name;
                Box2 boundingBox;
                std::shared_ptr<graphics::MeshBuffer> meshBuffer;
                std::shared_ptr<graphics::Buffer> indexBuffer;
                std::shared_ptr<graphics::Buffer> vertexBuffer;
            };

            struct Animation
            {
                std::string name;
                std::vector<Frame> frames;
                float frameInterval = 0.1F;
            };

            std::map<std::string, Animation> animations;
            std::shared_ptr<graphics::Texture> texture;
            std::shared_ptr<graphics::BlendState> blendState;
            std::shared_ptr<graphics::Shader> shader;
        };
    } // namespace scene
} // namespace ouzel
