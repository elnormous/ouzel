// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SpriteData.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        SpriteData::Frame::Frame(const std::string& frameName,
                                 const Size2<float>& textureSize,
                                 const Rect<float>& frameRectangle,
                                 bool rotated,
                                 const Size2<float>& sourceSize,
                                 const Vector2<float>& sourceOffset,
                                 const Vector2<float>& pivot):
            name(frameName)
        {
            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};
            indexCount = static_cast<uint32_t>(indices.size());

            Vector2<float> textCoords[4];
            Vector2<float> finalOffset(-sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
                                       -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1]));

            if (!rotated)
            {
                Vector2<float> leftTop(frameRectangle.position.v[0] / textureSize.v[0],
                                frameRectangle.position.v[1] / textureSize.v[1]);

                Vector2<float> rightBottom((frameRectangle.position.v[0] + frameRectangle.size.v[0]) / textureSize.v[0],
                                    (frameRectangle.position.v[1] + frameRectangle.size.v[1]) / textureSize.v[1]);

                textCoords[0] = Vector2<float>(leftTop.v[0], rightBottom.v[1]);
                textCoords[1] = Vector2<float>(rightBottom.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2<float>(leftTop.v[0], leftTop.v[1]);
                textCoords[3] = Vector2<float>(rightBottom.v[0], leftTop.v[1]);
            }
            else
            {
                Vector2<float> leftTop = Vector2<float>(frameRectangle.position.v[0] / textureSize.v[0],
                                          frameRectangle.position.v[1] / textureSize.v[1]);

                Vector2<float> rightBottom = Vector2<float>((frameRectangle.position.v[0] + frameRectangle.size.v[1]) / textureSize.v[0],
                                              (frameRectangle.position.v[1] + frameRectangle.size.v[0]) / textureSize.v[1]);

                textCoords[0] = Vector2<float>(leftTop.v[0], leftTop.v[1]);
                textCoords[1] = Vector2<float>(leftTop.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2<float>(rightBottom.v[0], leftTop.v[1]);
                textCoords[3] = Vector2<float>(rightBottom.v[0], rightBottom.v[1]);
            }

            std::vector<graphics::Vertex> vertices = {
                graphics::Vertex(Vector3<float>(finalOffset.v[0], finalOffset.v[1], 0.0F), Color::WHITE,
                                 textCoords[0], Vector3<float>(0.0F, 0.0F, -1.0F)),
                graphics::Vertex(Vector3<float>(finalOffset.v[0] + frameRectangle.size.v[0], finalOffset.v[1], 0.0F), Color::WHITE,
                                 textCoords[1], Vector3<float>(0.0F, 0.0F, -1.0F)),
                graphics::Vertex(Vector3<float>(finalOffset.v[0], finalOffset.v[1] + frameRectangle.size.v[1], 0.0F), Color::WHITE,
                                 textCoords[2], Vector3<float>(0.0F, 0.0F, -1.0F)),
                graphics::Vertex(Vector3<float>(finalOffset.v[0] + frameRectangle.size.v[0], finalOffset.v[1] + frameRectangle.size.v[1], 0.0F), Color::WHITE,
                                 textCoords[3], Vector3<float>(0.0F, 0.0F, -1.0F))
            };

            boundingBox.min = finalOffset;
            boundingBox.max = finalOffset + Vector2<float>(frameRectangle.size.v[0], frameRectangle.size.v[1]);

            indexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                             graphics::Buffer::Usage::INDEX, 0,
                                                             indices.data(),
                                                             static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                              graphics::Buffer::Usage::VERTEX,0,
                                                              vertices.data(),
                                                              static_cast<uint32_t>(getVectorSize(vertices)));
        }

        SpriteData::Frame::Frame(const std::string& frameName,
                                 const std::vector<uint16_t>& indices,
                                 const std::vector<graphics::Vertex>& vertices):
            name(frameName)
        {
            indexCount = static_cast<uint32_t>(indices.size());

            for (const graphics::Vertex& vertex : vertices)
                boundingBox.insertPoint(Vector2<float>(vertex.position));

            indexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                             graphics::Buffer::Usage::INDEX, 0,
                                                             indices.data(),
                                                             static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                              graphics::Buffer::Usage::VERTEX, 0,
                                                              vertices.data(),
                                                              static_cast<uint32_t>(getVectorSize(vertices)));
        }

        SpriteData::Frame::Frame(const std::string& frameName,
                                 const std::vector<uint16_t>& indices,
                                 const std::vector<graphics::Vertex>& vertices,
                                 const Rect<float>& frameRectangle,
                                 const Size2<float>& sourceSize,
                                 const Vector2<float>& sourceOffset,
                                 const Vector2<float>& pivot):
            name(frameName)
        {
            indexCount = static_cast<uint32_t>(indices.size());

            for (const graphics::Vertex& vertex : vertices)
                boundingBox.insertPoint(Vector2<float>(vertex.position));

            // TODO: fix
            Vector2<float> finalOffset(-sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
                                -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1]));

            indexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                             graphics::Buffer::Usage::INDEX, 0,
                                                             indices.data(),
                                                             static_cast<uint32_t>(getVectorSize(indices)));

            vertexBuffer = std::make_shared<graphics::Buffer>(*engine->getRenderer(),
                                                              graphics::Buffer::Usage::VERTEX, 0,
                                                              vertices.data(),
                                                              static_cast<uint32_t>(getVectorSize(vertices)));
        }
    } // namespace scene
} // namespace ouzel
