// Ouzel by Elviss Strazdins

#include <algorithm>
#include "SpriteLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    SpriteLoader::SpriteLoader():
        Loader{Asset::Type::sprite}
    {
    }

    bool SpriteLoader::loadAsset(Cache& cache,
                                 Bundle& bundle,
                                 const std::string& name,
                                 const std::vector<std::byte>& data,
                                 const Asset::Options& options)
    {
        scene::SpriteData spriteData;

        const auto d = json::parse(data);

        if (!d.hasMember("meta") ||
            !d.hasMember("frames"))
            return false;

        const json::Value& metaObject = d["meta"];

        const auto imageFilename = metaObject["image"].as<std::string>();
        spriteData.texture = cache.getTexture(imageFilename);
        if (!spriteData.texture)
        {
            bundle.loadAsset(Asset::Type::image, imageFilename, imageFilename, options);
            spriteData.texture = cache.getTexture(imageFilename);
        }

        if (!spriteData.texture)
            return false;

        const Size<float, 2> textureSize{
            static_cast<float>(spriteData.texture->getSize().v[0]),
            static_cast<float>(spriteData.texture->getSize().v[1])
        };

        const json::Value& framesArray = d["frames"];

        scene::SpriteData::Animation animation;

        animation.frames.reserve(framesArray.getSize());

        for (const json::Value& frameObject : framesArray)
        {
            const auto filename = frameObject["filename"].as<std::string>();

            const json::Value& frameRectangleObject = frameObject["frame"];

            const Rect<float> frameRectangle{
                frameRectangleObject["x"].as<float>(),
                frameRectangleObject["y"].as<float>(),
                frameRectangleObject["w"].as<float>(),
                frameRectangleObject["h"].as<float>()
            };

            const json::Value& sourceSizeObject = frameObject["sourceSize"];

            const Size<float, 2> sourceSize{
                sourceSizeObject["w"].as<float>(),
                sourceSizeObject["h"].as<float>()
            };

            const json::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

            const Vector<float, 2> sourceOffset{
                spriteSourceSizeObject["x"].as<float>(),
                spriteSourceSizeObject["y"].as<float>()
            };

            const json::Value& pivotObject = frameObject["pivot"];

            const Vector<float, 2> pivot{
                pivotObject["x"].as<float>(),
                pivotObject["y"].as<float>()
            };

            if (frameObject.hasMember("vertices") &&
                frameObject.hasMember("verticesUV") &&
                frameObject.hasMember("triangles"))
            {
                std::vector<std::uint16_t> indices;

                const json::Value& trianglesObject = frameObject["triangles"];

                for (const json::Value& triangleObject : trianglesObject)
                    for (const json::Value& indexObject : triangleObject)
                        indices.push_back(static_cast<std::uint16_t>(indexObject.as<std::uint32_t>()));

                // reverse the vertices, so that they are counterclockwise
                std::reverse(indices.begin(), indices.end());

                std::vector<graphics::Vertex> vertices;

                const json::Value& verticesObject = frameObject["vertices"];
                const json::Value& verticesUVObject = frameObject["verticesUV"];

                const Vector<float, 2> finalOffset{
                    -sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
                    -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1])
                };

                for (std::size_t vertexIndex = 0; vertexIndex < verticesObject.getSize(); ++vertexIndex)
                {
                    const json::Value& vertexObject = verticesObject[vertexIndex];
                    const json::Value& vertexUVObject = verticesUVObject[vertexIndex];

                    vertices.emplace_back(Vector<float, 3>{static_cast<float>(vertexObject[0].as<std::int32_t>()) + finalOffset.v[0],
                                                           -static_cast<float>(vertexObject[1].as<std::int32_t>()) - finalOffset.v[1], 0.0F},
                                          Color::white(),
                                          Vector<float, 2>{static_cast<float>(vertexUVObject[0].as<std::int32_t>()) / textureSize.v[0],
                                                           static_cast<float>(vertexUVObject[1].as<std::int32_t>()) / textureSize.v[1]},
                                          Vector<float, 3>{0.0F, 0.0F, -1.0F});
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
