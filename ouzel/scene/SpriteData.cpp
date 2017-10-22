// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "json.hpp"
#include "SpriteData.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace scene
    {
        bool SpriteData::load(const std::string& filename, bool mipmaps)
        {
            std::vector<uint8_t> data;
            if (!sharedEngine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            nlohmann::json document = nlohmann::json::parse(data);

            if (document.find("meta") == document.end() ||
                document.find("frames") == document.end())
            {
                return false;
            }
            
            const nlohmann::json& metaObject = document["meta"];

            texture = sharedEngine->getCache()->getTexture(metaObject["image"].get<std::string>(), false, mipmaps);

            if (!texture)
            {
                return false;
            }

            const nlohmann::json& framesArray = document["frames"];

            frames.reserve(framesArray.size());

            for (const nlohmann::json& frameObject : framesArray)
            {
                std::string name = frameObject["filename"];

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

                    // reverse the vertices, so that they are counterclockwise
                    std::reverse(indices.begin(), indices.end());

                    std::vector<graphics::VertexPCT> vertices;

                    const nlohmann::json& verticesObject = frameObject["vertices"];
                    const nlohmann::json& verticesUVObject = frameObject["verticesUV"];

                    const Size2& textureSize = texture->getSize();

                    Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                        -sourceSize.height * pivot.y + (sourceSize.height - frameRectangle.size.height - sourceOffset.y));

                    for (size_t vertexIndex = 0; vertexIndex < verticesObject.size(); ++vertexIndex)
                    {
                        const nlohmann::json& vertexObject = verticesObject[vertexIndex];
                        const nlohmann::json& vertexUVObject = verticesUVObject[vertexIndex];

                        vertices.push_back(graphics::VertexPCT(Vector3(static_cast<float>(vertexObject[0].get<int32_t>()) + finalOffset.x,
                                                                       -static_cast<float>(vertexObject[1].get<int32_t>()) - finalOffset.y,
                                                                       0.0f),
                                                               Color::WHITE,
                                                               Vector2(static_cast<float>(vertexUVObject[0].get<int32_t>()) / textureSize.width,
                                                                       static_cast<float>(vertexUVObject[1].get<int32_t>()) / textureSize.height)));
                    }

                    frames.push_back(SpriteFrame(name, indices, vertices, frameRectangle, sourceSize, sourceOffset, pivot));
                }
                else
                {
                    bool rotated = frameObject["rotated"].get<bool>();

                    frames.push_back(SpriteFrame(name, texture->getSize(), frameRectangle, rotated, sourceSize, sourceOffset, pivot));
                }
            }

            return true;
        }
    } // namespace scene
} // namespace ouzel
