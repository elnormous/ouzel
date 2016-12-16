// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/document.h>
#include "SpriteFrame.h"
#include "core/Engine.h"
#include "core/Application.h"
#include "core/Cache.h"
#include "files/FileSystem.h"
#include "graphics/Texture.h"
#include "graphics/MeshBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/VertexBuffer.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace scene
    {
        std::vector<SpriteFrame> SpriteFrame::loadSpriteFrames(const std::string& filename, bool mipmaps)
        {
            std::vector<SpriteFrame> frames;

            std::vector<uint8_t> data;
            if (!sharedApplication->getFileSystem()->loadFile(filename, data))
            {
                return frames;
            }

            rapidjson::MemoryStream is(reinterpret_cast<char*>(data.data()), data.size());

            rapidjson::Document document;
            document.ParseStream<0>(is);

            if (document.HasParseError())
            {
                Log(Log::Level::ERR) << "Failed to parse " << filename;
                return frames;
            }

            const rapidjson::Value& metaObject = document["meta"];

            graphics::TexturePtr texture = sharedEngine->getCache()->getTexture(metaObject["image"].GetString(), false, mipmaps);

            const rapidjson::Value& framesArray = document["frames"];

            frames.reserve(framesArray.Size());

            for (rapidjson::SizeType index = 0; index < framesArray.Size(); ++index)
            {
                const rapidjson::Value& frameObject = framesArray[index];

                const rapidjson::Value& frameRectangleObject = frameObject["frame"];

                Rectangle frameRectangle(static_cast<float>(frameRectangleObject["x"].GetInt()),
                                         static_cast<float>(frameRectangleObject["y"].GetInt()),
                                         static_cast<float>(frameRectangleObject["w"].GetInt()),
                                         static_cast<float>(frameRectangleObject["h"].GetInt()));

                const rapidjson::Value& sourceSizeObject = frameObject["sourceSize"];

                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].GetInt()),
                                 static_cast<float>(sourceSizeObject["h"].GetInt()));

                const rapidjson::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

                Vector2 sourceOffset(static_cast<float>(spriteSourceSizeObject["x"].GetInt()),
                                     static_cast<float>(spriteSourceSizeObject["y"].GetInt()));

                const rapidjson::Value& pivotObject = frameObject["pivot"];

                Vector2 pivot(pivotObject["x"].GetFloat(),
                              pivotObject["y"].GetFloat());

                if (frameObject.HasMember("vertices") &&
                    frameObject.HasMember("verticesUV") &&
                    frameObject.HasMember("triangles"))
                {
                    std::vector<uint16_t> indices;

                    const rapidjson::Value& trianglesObject = frameObject["triangles"];

                    for (rapidjson::SizeType triangleIndex = 0; triangleIndex < trianglesObject.Size(); ++triangleIndex)
                    {
                        const rapidjson::Value& triangleObject = trianglesObject[triangleIndex];

                        for (rapidjson::SizeType i = 0; i < triangleObject.Size(); ++i)
                        {
                            indices.push_back(static_cast<uint16_t>(triangleObject[i].GetUint()));
                        }
                    }

                    std::vector<graphics::VertexPCT> vertices;

                    const rapidjson::Value& verticesObject = frameObject["vertices"];
                    const rapidjson::Value& verticesUVObject = frameObject["verticesUV"];

                    const Size2& textureSize = texture->getSize();

                    Vector2 finalOffset(-sourceSize.width * pivot.v[0] + sourceOffset.v[0],
                                        -sourceSize.height * pivot.v[1] + (sourceSize.height - frameRectangle.size.height - sourceOffset.v[1]));

                    for (rapidjson::SizeType vertexIndex = 0; vertexIndex < verticesObject.Size(); ++vertexIndex)
                    {
                        const rapidjson::Value& vertexObject = verticesObject[vertexIndex];
                        const rapidjson::Value& vertexUVObject = verticesUVObject[vertexIndex];

                        vertices.push_back(graphics::VertexPCT(Vector3(static_cast<float>(vertexObject[0].GetInt()) + finalOffset.v[0],
                                                                       -static_cast<float>(vertexObject[1].GetInt()) - finalOffset.v[1],
                                                                       0.0f),
                                                               Color::WHITE,
                                                               Vector2(static_cast<float>(vertexUVObject[0].GetInt()) / textureSize.width,
                                                                       static_cast<float>(vertexUVObject[1].GetInt()) / textureSize.height)));
                    }

                    frames.push_back(SpriteFrame(texture, indices, vertices, frameRectangle, sourceSize, sourceOffset, pivot));
                }
                else
                {
                    bool rotated = frameObject["rotated"].GetBool();

                    frames.push_back(SpriteFrame(texture, frameRectangle, rotated, sourceSize, sourceOffset, pivot));
                }
            }

            return frames;
        }

        SpriteFrame::SpriteFrame(const graphics::TexturePtr& pTexture,
                                 const Rectangle& frameRectangle,
                                 bool rotated,
                                 const Size2& sourceSize,
                                 const Vector2& sourceOffset,
                                 const Vector2& pivot)
        {
            texture = pTexture;

            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};

            Vector2 textCoords[4];
            Vector2 finalOffset(-sourceSize.width * pivot.v[0] + sourceOffset.v[0],
                                -sourceSize.height * pivot.v[1] + (sourceSize.height - frameRectangle.size.height - sourceOffset.v[1]));

            const Size2& textureSize = texture->getSize();

            if (!rotated)
            {
                Vector2 leftTop(frameRectangle.position.v[0] / textureSize.width,
                                frameRectangle.position.v[1] / textureSize.height);

                Vector2 rightBottom((frameRectangle.position.v[0] + frameRectangle.size.width) / textureSize.width,
                                    (frameRectangle.position.v[1] + frameRectangle.size.height) / textureSize.height);

                textCoords[0] = Vector2(leftTop.v[0], rightBottom.v[1]);
                textCoords[1] = Vector2(rightBottom.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2(leftTop.v[0], leftTop.v[1]);
                textCoords[3] = Vector2(rightBottom.v[0], leftTop.v[1]);
            }
            else
            {
                Vector2 leftTop = Vector2(frameRectangle.position.v[0] / textureSize.width,
                                          frameRectangle.position.v[1] / textureSize.height);

                Vector2 rightBottom = Vector2((frameRectangle.position.v[0] + frameRectangle.size.height) / textureSize.width,
                                              (frameRectangle.position.v[1] + frameRectangle.size.width) / textureSize.height);

                textCoords[0] = Vector2(leftTop.v[0], leftTop.v[1]);
                textCoords[1] = Vector2(leftTop.v[0], rightBottom.v[1]);
                textCoords[2] = Vector2(rightBottom.v[0], leftTop.v[1]);
                textCoords[3] = Vector2(rightBottom.v[0], rightBottom.v[1]);
            }

            std::vector<graphics::VertexPCT> vertices = {
                graphics::VertexPCT(Vector3(finalOffset.v[0], finalOffset.v[1], 0.0f), Color::WHITE, textCoords[0]),
                graphics::VertexPCT(Vector3(finalOffset.v[0] + frameRectangle.size.width, finalOffset.v[1], 0.0f), Color::WHITE, textCoords[1]),
                graphics::VertexPCT(Vector3(finalOffset.v[0], finalOffset.v[1] + frameRectangle.size.height, 0.0f),  Color::WHITE, textCoords[2]),
                graphics::VertexPCT(Vector3(finalOffset.v[0] + frameRectangle.size.width, finalOffset.v[1] + frameRectangle.size.height, 0.0f),  Color::WHITE, textCoords[3])
            };

            boundingBox.set(finalOffset, finalOffset + Vector2(frameRectangle.size.width, frameRectangle.size.height));

            rectangle = Rectangle(finalOffset.v[0], finalOffset.v[1],
                                  sourceSize.width, sourceSize.height);

            indexBuffer = sharedEngine->getRenderer()->createIndexBuffer();
            indexBuffer->initFromBuffer(indices.data(), sizeof(uint16_t),
                                        static_cast<uint32_t>(indices.size()), false);

            vertexBuffer = sharedEngine->getRenderer()->createVertexBuffer();
            vertexBuffer->initFromBuffer(vertices.data(), graphics::VertexPCT::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), true);

            meshBuffer = sharedEngine->getRenderer()->createMeshBuffer();
            meshBuffer->init(indexBuffer, vertexBuffer);
        }

        SpriteFrame::SpriteFrame(const graphics::TexturePtr& pTexture,
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

            Vector2 finalOffset(-sourceSize.width * pivot.v[0] + sourceOffset.v[0],
                                -sourceSize.height * pivot.v[1] + (sourceSize.height - frameRectangle.size.height - sourceOffset.v[1]));

            rectangle = Rectangle(finalOffset.v[0], finalOffset.v[1],
                                  sourceSize.width, sourceSize.height);

            indexBuffer = sharedEngine->getRenderer()->createIndexBuffer();
            indexBuffer->initFromBuffer(indices.data(), sizeof(uint16_t),
                                        static_cast<uint32_t>(indices.size()), false);

            vertexBuffer = sharedEngine->getRenderer()->createVertexBuffer();
            vertexBuffer->initFromBuffer(vertices.data(), graphics::VertexPCT::ATTRIBUTES,
                                         static_cast<uint32_t>(vertices.size()), true);

            meshBuffer = sharedEngine->getRenderer()->createMeshBuffer();
            meshBuffer->init(indexBuffer, vertexBuffer);
        }

    } // scene
} // ouzel
