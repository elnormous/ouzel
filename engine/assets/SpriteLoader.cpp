// Ouzel by Elviss Strazdins

#include <algorithm>
#include "SpriteLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    SpriteLoader::SpriteLoader(Cache& initCache):
        Loader(initCache, Type::sprite)
    {
    }

    bool SpriteLoader::loadAsset(Bundle& bundle,
                                 const std::string& name,
                                 const std::vector<std::byte>& data,
                                 bool mipmaps)
    {
        scene::SpriteData spriteData;

        const json::Value d = json::parse(data);

        if (!d.hasMember("meta") ||
            !d.hasMember("frames"))
            return false;

        const json::Value& metaObject = d["meta"];

        const auto imageFilename = metaObject["image"].as<std::string>();
        spriteData.texture = cache.getTexture(imageFilename);
        if (!spriteData.texture)
        {
            bundle.loadAsset(Type::image, imageFilename, imageFilename, mipmaps);
            spriteData.texture = cache.getTexture(imageFilename);
        }

        if (!spriteData.texture)
            return false;

        const Size2F textureSize(static_cast<float>(spriteData.texture->getSize().v[0]),
                                       static_cast<float>(spriteData.texture->getSize().v[1]));

        const json::Value& framesArray = d["frames"];

        scene::SpriteData::Animation animation;

        animation.frames.reserve(framesArray.getSize());

        for (const json::Value& frameObject : framesArray)
        {
            const auto filename = frameObject["filename"].as<std::string>();

            const json::Value& frameRectangleObject = frameObject["frame"];

            RectF frameRectangle(static_cast<float>(frameRectangleObject["x"].as<std::int32_t>()),
                                 static_cast<float>(frameRectangleObject["y"].as<std::int32_t>()),
                                 static_cast<float>(frameRectangleObject["w"].as<std::int32_t>()),
                                 static_cast<float>(frameRectangleObject["h"].as<std::int32_t>()));

            const json::Value& sourceSizeObject = frameObject["sourceSize"];

            Size2F sourceSize(static_cast<float>(sourceSizeObject["w"].as<std::int32_t>()),
                              static_cast<float>(sourceSizeObject["h"].as<std::int32_t>()));

            const json::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

            Vector2F sourceOffset(static_cast<float>(spriteSourceSizeObject["x"].as<std::int32_t>()),
                                  static_cast<float>(spriteSourceSizeObject["y"].as<std::int32_t>()));

            const json::Value& pivotObject = frameObject["pivot"];

            const Vector2F pivot(pivotObject["x"].as<float>(),
                                 pivotObject["y"].as<float>());

            if (frameObject.hasMember("vertices") &&
                frameObject.hasMember("verticesUV") &&
                frameObject.hasMember("triangles"))
            {
                std::vector<std::uint16_t> indices;

                const json::Value& trianglesObject = frameObject["triangles"];

                for (const json::Value& triangleObject : trianglesObject)
                {
                    for (const json::Value& indexObject : triangleObject)
                        indices.push_back(static_cast<std::uint16_t>(indexObject.as<std::uint32_t>()));
                }

                // reverse the vertices, so that they are counterclockwise
                std::reverse(indices.begin(), indices.end());

                std::vector<graphics::Vertex> vertices;

                const json::Value& verticesObject = frameObject["vertices"];
                const json::Value& verticesUVObject = frameObject["verticesUV"];

                Vector2F finalOffset(-sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
                                     -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1]));

                for (std::size_t vertexIndex = 0; vertexIndex < verticesObject.getSize(); ++vertexIndex)
                {
                    const json::Value& vertexObject = verticesObject[vertexIndex];
                    const json::Value& vertexUVObject = verticesUVObject[vertexIndex];

                    vertices.emplace_back(Vector3F{static_cast<float>(vertexObject[0].as<std::int32_t>()) + finalOffset.v[0],
                                                   -static_cast<float>(vertexObject[1].as<std::int32_t>()) - finalOffset.v[1], 0.0F},
                                          Color::white(),
                                          Vector2F{static_cast<float>(vertexUVObject[0].as<std::int32_t>()) / textureSize.v[0],
                                                   static_cast<float>(vertexUVObject[1].as<std::int32_t>()) / textureSize.v[1]},
                                          Vector3F{0.0F, 0.0F, -1.0F});
                }

                animation.frames.emplace_back(filename, indices, vertices, frameRectangle, sourceSize, sourceOffset, pivot);
            }
            else
            {
                const auto rotated = frameObject["rotated"].as<bool>();

                animation.frames.emplace_back(filename, textureSize, frameRectangle, rotated, sourceSize, sourceOffset, pivot);
            }
        }

        spriteData.animations[""] = std::move(animation);

        bundle.setSpriteData(name, spriteData);

        return true;
    }
}
