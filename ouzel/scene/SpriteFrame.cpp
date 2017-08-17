// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "json.hpp"
#include "SpriteFrame.hpp"
#include "core/Engine.hpp"
#include "core/Cache.hpp"
#include "files/FileSystem.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        SpriteFrame::SpriteFrame(const Size2& textureSize,
                                 const Rectangle& frameRectangle,
                                 bool rotated,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot)
        {
            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};

            Vector2 textCoords[4];
            Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                -sourceSize.height * pivot.y + (sourceSize.height - frameRectangle.size.height - sourceOffset.y));

            if (!rotated)
            {
                Vector2 leftTop(frameRectangle.position.x / textureSize.width,
                                frameRectangle.position.y / textureSize.height);

                Vector2 rightBottom((frameRectangle.position.x + frameRectangle.size.width) / textureSize.width,
                                    (frameRectangle.position.y + frameRectangle.size.height) / textureSize.height);

                textCoords[0] = Vector2(leftTop.x, rightBottom.y);
                textCoords[1] = Vector2(rightBottom.x, rightBottom.y);
                textCoords[2] = Vector2(leftTop.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, leftTop.y);
            }
            else
            {
                Vector2 leftTop = Vector2(frameRectangle.position.x / textureSize.width,
                                          frameRectangle.position.y / textureSize.height);

                Vector2 rightBottom = Vector2((frameRectangle.position.x + frameRectangle.size.height) / textureSize.width,
                                              (frameRectangle.position.y + frameRectangle.size.width) / textureSize.height);

                textCoords[0] = Vector2(leftTop.x, leftTop.y);
                textCoords[1] = Vector2(leftTop.x, rightBottom.y);
                textCoords[2] = Vector2(rightBottom.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, rightBottom.y);
            }

            std::vector<graphics::VertexPCT> vertices = {
                graphics::VertexPCT(Vector3(finalOffset.x, finalOffset.y, 0.0f), Color::WHITE, textCoords[0]),
                graphics::VertexPCT(Vector3(finalOffset.x + frameRectangle.size.width, finalOffset.y, 0.0f), Color::WHITE, textCoords[1]),
                graphics::VertexPCT(Vector3(finalOffset.x, finalOffset.y + frameRectangle.size.height, 0.0f),  Color::WHITE, textCoords[2]),
                graphics::VertexPCT(Vector3(finalOffset.x + frameRectangle.size.width, finalOffset.y + frameRectangle.size.height, 0.0f),  Color::WHITE, textCoords[3])
            };

            boundingBox.set(finalOffset, finalOffset + Vector2(frameRectangle.size.width, frameRectangle.size.height));

            rectangle = Rectangle(finalOffset.x, finalOffset.y,
                                  sourceSize.width, sourceSize.height);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), 0);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);
        }

        SpriteFrame::SpriteFrame(const std::vector<uint16_t>& indices,
                                 const std::vector<graphics::VertexPCT>& vertices,
                                 const Rectangle& frameRectangle,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot)
        {
            for (const graphics::VertexPCT& vertex : vertices)
            {
                boundingBox.insertPoint(vertex.position);
            }

            Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                -sourceSize.height * pivot.y + (sourceSize.height - frameRectangle.size.height - sourceOffset.y));

            rectangle = Rectangle(finalOffset.x, finalOffset.y,
                                  sourceSize.width, sourceSize.height);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), 0);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);
        }

    } // scene
} // ouzel
