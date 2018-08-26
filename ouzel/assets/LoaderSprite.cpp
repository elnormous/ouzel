// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "LoaderSprite.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "scene/SpriteData.hpp"
#include "utils/JSON.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderSprite::LoaderSprite(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool LoaderSprite::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool)
        {
            scene::SpriteData spriteData;

            json::Data document(data);

            if (!document.hasMember("meta") ||
                !document.hasMember("frames"))
                return false;

            const json::Value& metaObject = document["meta"];

            spriteData.texture = cache.getTexture(metaObject["image"].as<std::string>());

            if (!spriteData.texture)
                return false;

            const json::Value& framesArray = document["frames"];

            scene::SpriteData::Animation animation;

            animation.frames.reserve(framesArray.getSize());

            for (const json::Value& frameObject : framesArray.as<json::Value::Array>())
            {
                std::string name = frameObject["filename"].as<std::string>();

                const json::Value& frameRectangleObject = frameObject["frame"];

                Rect frameRectangle(static_cast<float>(frameRectangleObject["x"].as<int32_t>()),
                                    static_cast<float>(frameRectangleObject["y"].as<int32_t>()),
                                    static_cast<float>(frameRectangleObject["w"].as<int32_t>()),
                                    static_cast<float>(frameRectangleObject["h"].as<int32_t>()));

                const json::Value& sourceSizeObject = frameObject["sourceSize"];

                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].as<int32_t>()),
                                 static_cast<float>(sourceSizeObject["h"].as<int32_t>()));

                const json::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

                Vector2 sourceOffset(static_cast<float>(spriteSourceSizeObject["x"].as<int32_t>()),
                                     static_cast<float>(spriteSourceSizeObject["y"].as<int32_t>()));

                const json::Value& pivotObject = frameObject["pivot"];

                Vector2 pivot(pivotObject["x"].as<float>(),
                              pivotObject["y"].as<float>());

                if (frameObject.hasMember("vertices") &&
                    frameObject.hasMember("verticesUV") &&
                    frameObject.hasMember("triangles"))
                {
                    std::vector<uint16_t> indices;

                    const json::Value& trianglesObject = frameObject["triangles"];

                    for (const json::Value& triangleObject : trianglesObject.as<json::Value::Array>())
                    {
                        for (const json::Value& indexObject : triangleObject.as<json::Value::Array>())
                            indices.push_back(static_cast<uint16_t>(indexObject.as<uint32_t>()));
                    }

                    // reverse the vertices, so that they are counterclockwise
                    std::reverse(indices.begin(), indices.end());

                    std::vector<graphics::Vertex> vertices;

                    const json::Value& verticesObject = frameObject["vertices"];
                    const json::Value& verticesUVObject = frameObject["verticesUV"];

                    const Size2& textureSize = spriteData.texture->getSize();

                    Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                        -sourceSize.height * pivot.y + (sourceSize.height - frameRectangle.size.height - sourceOffset.y));

                    for (size_t vertexIndex = 0; vertexIndex < verticesObject.getSize(); ++vertexIndex)
                    {
                        const json::Value& vertexObject = verticesObject[vertexIndex];
                        const json::Value& vertexUVObject = verticesUVObject[vertexIndex];

                        vertices.push_back(graphics::Vertex(Vector3(static_cast<float>(vertexObject[0].as<int32_t>()) + finalOffset.x,
                                                                    -static_cast<float>(vertexObject[1].as<int32_t>()) - finalOffset.y,
                                                                    0.0F),
                                                            Color::WHITE,
                                                            Vector2(static_cast<float>(vertexUVObject[0].as<int32_t>()) / textureSize.width,
                                                                    static_cast<float>(vertexUVObject[1].as<int32_t>()) / textureSize.height),
                                                            Vector3(0.0F, 0.0F, -1.0F)));
                    }

                    animation.frames.push_back(scene::SpriteData::Frame(name, indices, vertices, frameRectangle, sourceSize, sourceOffset, pivot));
                }
                else
                {
                    bool rotated = frameObject["rotated"].as<bool>();

                    animation.frames.push_back(scene::SpriteData::Frame(name, spriteData.texture->getSize(), frameRectangle, rotated, sourceSize, sourceOffset, pivot));
                }
            }

            spriteData.animations[""] = std::move(animation);

            bundle.setSpriteData(filename, spriteData);

            return true;
        }
    } // namespace assets
} // namespace ouzel
