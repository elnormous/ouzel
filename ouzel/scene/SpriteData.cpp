// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "SpriteData.hpp"
#include "core/Engine.hpp"
#include "utils/JSON.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        bool SpriteData::init(const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data;
            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data, mipmaps);
        }

        bool SpriteData::init(const std::vector<uint8_t>& data, bool mipmaps)
        {
            json::Data document;

            if (!document.init(data))
            {
                return false;
            }

            if (!document.hasMember("meta") ||
                !document.hasMember("frames"))
            {
                return false;
            }

            const json::Value& metaObject = document["meta"];

            texture = engine->getCache()->getTexture(metaObject["image"].asString(), mipmaps);

            if (!texture)
            {
                return false;
            }

            const json::Value& framesArray = document["frames"];

            Animation animation;

            animation.frames.reserve(framesArray.getSize());

            for (const json::Value& frameObject : framesArray.asArray())
            {
                std::string name = frameObject["filename"].asString();

                const json::Value& frameRectangleObject = frameObject["frame"];

                Rect frameRectangle(static_cast<float>(frameRectangleObject["x"].asInt32()),
                                    static_cast<float>(frameRectangleObject["y"].asInt32()),
                                    static_cast<float>(frameRectangleObject["w"].asInt32()),
                                    static_cast<float>(frameRectangleObject["h"].asInt32()));

                const json::Value& sourceSizeObject = frameObject["sourceSize"];

                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].asInt32()),
                                 static_cast<float>(sourceSizeObject["h"].asInt32()));

                const json::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

                Vector2 sourceOffset(static_cast<float>(spriteSourceSizeObject["x"].asInt32()),
                                     static_cast<float>(spriteSourceSizeObject["y"].asInt32()));

                const json::Value& pivotObject = frameObject["pivot"];

                Vector2 pivot(pivotObject["x"].asFloat(),
                              pivotObject["y"].asFloat());

                if (frameObject.hasMember("vertices") &&
                    frameObject.hasMember("verticesUV") &&
                    frameObject.hasMember("triangles"))
                {
                    std::vector<uint16_t> indices;

                    const json::Value& trianglesObject = frameObject["triangles"];

                    for (const json::Value& triangleObject : trianglesObject.asArray())
                    {
                        for (const json::Value& indexObject : triangleObject.asArray())
                        {
                            indices.push_back(static_cast<uint16_t>(indexObject.asUInt32()));
                        }
                    }

                    // reverse the vertices, so that they are counterclockwise
                    std::reverse(indices.begin(), indices.end());

                    std::vector<graphics::Vertex> vertices;

                    const json::Value& verticesObject = frameObject["vertices"];
                    const json::Value& verticesUVObject = frameObject["verticesUV"];

                    const Size2& textureSize = texture->getSize();

                    Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                        -sourceSize.height * pivot.y + (sourceSize.height - frameRectangle.size.height - sourceOffset.y));

                    for (size_t vertexIndex = 0; vertexIndex < verticesObject.getSize(); ++vertexIndex)
                    {
                        const json::Value& vertexObject = verticesObject[vertexIndex];
                        const json::Value& vertexUVObject = verticesUVObject[vertexIndex];

                        vertices.push_back(graphics::Vertex(Vector3(static_cast<float>(vertexObject[0].asInt32()) + finalOffset.x,
                                                                    -static_cast<float>(vertexObject[1].asInt32()) - finalOffset.y,
                                                                    0.0f),
                                                            Color::WHITE,
                                                            Vector2(static_cast<float>(vertexUVObject[0].asInt32()) / textureSize.width,
                                                                    static_cast<float>(vertexUVObject[1].asInt32()) / textureSize.height),
                                                            Vector3(0.0f, 0.0f, -1.0f)));
                    }

                    animation.frames.push_back(Frame(name, indices, vertices, frameRectangle, sourceSize, sourceOffset, pivot));
                }
                else
                {
                    bool rotated = frameObject["rotated"].asBool();

                    animation.frames.push_back(Frame(name, texture->getSize(), frameRectangle, rotated, sourceSize, sourceOffset, pivot));
                }
            }

            animations[""] = std::move(animation);

            return true;
        }

        SpriteData::Frame::Frame(const std::string& frameName,
                                 const Size2& textureSize,
                                 const Rect& frameRectangle,
                                 bool rotated,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot):
        name(frameName)
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

            std::vector<graphics::Vertex> vertices = {
                graphics::Vertex(Vector3(finalOffset.x, finalOffset.y, 0.0f), Color::WHITE,
                                 textCoords[0], Vector3(0.0f, 0.0f, -1.0f)),
                graphics::Vertex(Vector3(finalOffset.x + frameRectangle.size.width, finalOffset.y, 0.0f), Color::WHITE,
                                 textCoords[1], Vector3(0.0f, 0.0f, -1.0f)),
                graphics::Vertex(Vector3(finalOffset.x, finalOffset.y + frameRectangle.size.height, 0.0f),  Color::WHITE,
                                 textCoords[2], Vector3(0.0f, 0.0f, -1.0f)),
                graphics::Vertex(Vector3(finalOffset.x + frameRectangle.size.width, finalOffset.y + frameRectangle.size.height, 0.0f),  Color::WHITE,
                                 textCoords[3], Vector3(0.0f, 0.0f, -1.0f))
            };

            boundingBox.set(finalOffset, finalOffset + Vector2(frameRectangle.size.width, frameRectangle.size.height));

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), 0);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, vertexBuffer);
        }

        SpriteData::Frame::Frame(const std::string& frameName,
                                 const std::vector<uint16_t>& indices,
                                 const std::vector<graphics::Vertex>& vertices,
                                 const Rect& frameRectangle,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot):
            name(frameName)
        {
            for (const graphics::Vertex& vertex : vertices)
            {
                boundingBox.insertPoint(vertex.position);
            }

            Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                -sourceSize.height * pivot.y + (sourceSize.height - frameRectangle.size.height - sourceOffset.y));

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), 0);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, vertexBuffer);
        }
    } // namespace scene
} // namespace ouzel
