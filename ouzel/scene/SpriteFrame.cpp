// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "json.hpp"
#include "SpriteFrame.hpp"
#include "core/Engine.hpp"
#include "core/Cache.hpp"
#include "files/FileSystem.hpp"
#include "graphics/TextureInterface.hpp"
#include "graphics/MeshBufferInterface.hpp"
#include "graphics/BufferInterface.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace scene
    {
        std::vector<SpriteFrame> SpriteFrame::loadSpriteFrames(const std::string& filename, bool mipmaps)
        {
            std::vector<SpriteFrame> frames;

            std::vector<uint8_t> data;
            if (!sharedEngine->getFileSystem()->readFile(filename, data))
            {
                return frames;
            }

            nlohmann::json document = nlohmann::json::parse(data);

            const nlohmann::json& metaObject = document["meta"];

            std::shared_ptr<graphics::Texture> texture = sharedEngine->getCache()->getTexture(metaObject["image"].get<std::string>(), false, mipmaps);

            const nlohmann::json& framesArray = document["frames"];

            frames.reserve(framesArray.size());

            for (const nlohmann::json& frameObject : framesArray)
            {
                const nlohmann::json& frameRectangleObject = frameObject["frame"];

                Rectangle frameRectangle(static_cast<float>(frameRectangleObject["x"].get<int32_t>()),
                                         static_cast<float>(frameRectangleObject["y"].get<int32_t>()),
                                         static_cast<float>(frameRectangleObject["w"].get<int32_t>()),
                                         static_cast<float>(frameRectangleObject["h"].get<int32_t>()));

                const nlohmann::json& sourceSizeObject = frameObject["sourceSize"];

                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].get<int32_t>()),
                                 static_cast<float>(sourceSizeObject["h"].get<int32_t>()));

                const nlohmann::json& spriteSourceSizeObject = frameObject["spriteSourceSize"];

                Vector2 sourceOffset(static_cast<float>(spriteSourceSizeObject["x"].get<int32_t>()),
                                     static_cast<float>(spriteSourceSizeObject["y"].get<int32_t>()));

                const nlohmann::json& pivotObject = frameObject["pivot"];

                Vector2 pivot(pivotObject["x"].get<float>(),
                              pivotObject["y"].get<float>());

                if (frameObject.find("vertices") != frameObject.end() &&
                    frameObject.find("verticesUV") != frameObject.end() &&
                    frameObject.find("triangles") != frameObject.end())
                {
                    std::vector<uint16_t> indices;

                    const nlohmann::json& trianglesObject = frameObject["triangles"];

                    for (const nlohmann::json& triangleObject : trianglesObject)
                    {
                        for (const nlohmann::json& indexObject : triangleObject)
                        {
                            indices.push_back(indexObject.get<uint16_t>());
                        }
                    }

                    std::vector<graphics::VertexPCT> vertices;

                    const nlohmann::json& verticesObject = frameObject["vertices"];
                    const nlohmann::json& verticesUVObject = frameObject["verticesUV"];

                    const Size2& textureSize = texture->getSize();

                    Vector2 finalOffset(-sourceSize.v[0] * pivot.x() + sourceOffset.x(),
                                        -sourceSize.v[1] * pivot.y() + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.y()));

                    for (size_t vertexIndex = 0; vertexIndex < verticesObject.size(); ++vertexIndex)
                    {
                        const nlohmann::json& vertexObject = verticesObject[vertexIndex];
                        const nlohmann::json& vertexUVObject = verticesUVObject[vertexIndex];

                        vertices.push_back(graphics::VertexPCT(Vector3(static_cast<float>(vertexObject[0].get<int32_t>()) + finalOffset.x(),
                                                                       -static_cast<float>(vertexObject[1].get<int32_t>()) - finalOffset.y(),
                                                                       0.0f),
                                                               Color::WHITE,
                                                               Vector2(static_cast<float>(vertexUVObject[0].get<int32_t>()) / textureSize.v[0],
                                                                       static_cast<float>(vertexUVObject[1].get<int32_t>()) / textureSize.v[1])));
                    }

                    frames.push_back(SpriteFrame(texture, indices, vertices, frameRectangle, sourceSize, sourceOffset, pivot));
                }
                else
                {
                    bool rotated = frameObject["rotated"].get<bool>();

                    frames.push_back(SpriteFrame(texture, frameRectangle, rotated, sourceSize, sourceOffset, pivot));
                }
            }

            return frames;
        }

        SpriteFrame::SpriteFrame(const std::shared_ptr<graphics::Texture>& pTexture,
                                 const Rectangle& frameRectangle,
                                 bool rotated,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot)
        {
            texture = pTexture;

            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};

            Vector2 textCoords[4];
            Vector2 finalOffset(-sourceSize.v[0] * pivot.x() + sourceOffset.x(),
                                -sourceSize.v[1] * pivot.y() + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.y()));

            const Size2& textureSize = texture->getSize();

            if (!rotated)
            {
                Vector2 leftTop(frameRectangle.position.x() / textureSize.v[0],
                                frameRectangle.position.y() / textureSize.v[1]);

                Vector2 rightBottom((frameRectangle.position.x() + frameRectangle.size.v[0]) / textureSize.v[0],
                                    (frameRectangle.position.y() + frameRectangle.size.v[1]) / textureSize.v[1]);

                textCoords[0] = Vector2(leftTop.x(), rightBottom.y());
                textCoords[1] = Vector2(rightBottom.x(), rightBottom.y());
                textCoords[2] = Vector2(leftTop.x(), leftTop.y());
                textCoords[3] = Vector2(rightBottom.x(), leftTop.y());
            }
            else
            {
                Vector2 leftTop = Vector2(frameRectangle.position.x() / textureSize.v[0],
                                          frameRectangle.position.y() / textureSize.v[1]);

                Vector2 rightBottom = Vector2((frameRectangle.position.x() + frameRectangle.size.v[1]) / textureSize.v[0],
                                              (frameRectangle.position.y() + frameRectangle.size.v[0]) / textureSize.v[1]);

                textCoords[0] = Vector2(leftTop.x(), leftTop.y());
                textCoords[1] = Vector2(leftTop.x(), rightBottom.y());
                textCoords[2] = Vector2(rightBottom.x(), leftTop.y());
                textCoords[3] = Vector2(rightBottom.x(), rightBottom.y());
            }

            std::vector<graphics::VertexPCT> vertices = {
                graphics::VertexPCT(Vector3(finalOffset.x(), finalOffset.y(), 0.0f), Color::WHITE, textCoords[0]),
                graphics::VertexPCT(Vector3(finalOffset.x() + frameRectangle.size.v[0], finalOffset.y(), 0.0f), Color::WHITE, textCoords[1]),
                graphics::VertexPCT(Vector3(finalOffset.x(), finalOffset.y() + frameRectangle.size.v[1], 0.0f),  Color::WHITE, textCoords[2]),
                graphics::VertexPCT(Vector3(finalOffset.x() + frameRectangle.size.v[0], finalOffset.y() + frameRectangle.size.v[1], 0.0f),  Color::WHITE, textCoords[3])
            };

            boundingBox.set(finalOffset, finalOffset + Vector2(frameRectangle.size.v[0], frameRectangle.size.v[1]));

            rectangle = Rectangle(finalOffset.x(), finalOffset.y(),
                                  sourceSize.v[0], sourceSize.v[1]);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), 0);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);
        }

        SpriteFrame::SpriteFrame(const std::shared_ptr<graphics::Texture>& pTexture,
                                 const std::vector<uint16_t>& indices,
                                 const std::vector<graphics::VertexPCT>& vertices,
                                 const Rectangle& frameRectangle,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot)
        {
            texture = pTexture;

            for (const graphics::VertexPCT& vertex : vertices)
            {
                boundingBox.insertPoint(vertex.position);
            }

            Vector2 finalOffset(-sourceSize.v[0] * pivot.x() + sourceOffset.x(),
                                -sourceSize.v[1] * pivot.y() + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.y()));

            rectangle = Rectangle(finalOffset.x(), finalOffset.y(),
                                  sourceSize.v[0], sourceSize.v[1]);

            indexBuffer = std::make_shared<graphics::Buffer>();
            indexBuffer->init(graphics::Buffer::Usage::INDEX, indices.data(), static_cast<uint32_t>(getVectorSize(indices)), 0);

            vertexBuffer = std::make_shared<graphics::Buffer>();
            vertexBuffer->init(graphics::Buffer::Usage::VERTEX, vertices.data(), static_cast<uint32_t>(getVectorSize(vertices)), 0);

            meshBuffer = std::make_shared<graphics::MeshBuffer>();
            meshBuffer->init(sizeof(uint16_t), indexBuffer, graphics::VertexPCT::ATTRIBUTES, vertexBuffer);
        }

    } // scene
} // ouzel
