// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SPRITEDATA_HPP
#define OUZEL_SPRITEDATA_HPP

#include <map>
#include <memory>
#include <vector>
#include "math/Box2.hpp"
#include "math/Rect.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Buffer.hpp"
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
                uint32_t getIndexCount() const { return indexCount; }
                inline const std::shared_ptr<graphics::Buffer>& getIndexBuffer() const { return indexBuffer; };
                inline const std::shared_ptr<graphics::Buffer>& getVertexBuffer() const { return vertexBuffer; };

            private:
                std::string name;
                Box2 boundingBox;
                uint32_t indexCount = 0;
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
            std::shared_ptr<graphics::BlendState> blendState;
            std::shared_ptr<graphics::Shader> shader;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SPRITEDATA_HPP
