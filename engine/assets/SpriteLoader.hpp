// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_SPRITELOADER_HPP
#define OUZEL_ASSETS_SPRITELOADER_HPP

#include <algorithm>
#include "Bundle.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    inline bool loadSprite(Cache&,
                           Bundle& bundle,
                           const std::string& name,
                           const std::vector<std::byte>& data,
                           const Asset::Options& options)
    {
        const auto d = json::parse(data);

        if (!d.hasMember("meta") ||
            !d.hasMember("frames"))
            return false;

        const auto& metaObject = d["meta"];
        const auto& imageFilename = metaObject["image"].as<std::string>();

        scene::SpriteData spriteData;
        spriteData.texture = bundle.getTexture(imageFilename);
        if (!spriteData.texture)
        {
            bundle.loadAsset(Asset::Type::image, imageFilename, imageFilename, options);
            spriteData.texture = bundle.getTexture(imageFilename);
        }

        if (!spriteData.texture)
            return false;

        const math::Size<float, 2> textureSize{
            static_cast<float>(spriteData.texture->getSize().v[0]),
            static_cast<float>(spriteData.texture->getSize().v[1])
        };

        const auto& framesArray = d["frames"];

        scene::SpriteData::Animation animation;

        animation.frames.reserve(framesArray.getSize());

        for (const auto& frameObject : framesArray)
        {
            const auto filename = frameObject["filename"].as<std::string>();

            const auto& frameRectangleObject = frameObject["frame"];

            const math::Rect<float> frameRectangle{
                frameRectangleObject["x"].as<float>(),
                frameRectangleObject["y"].as<float>(),
                frameRectangleObject["w"].as<float>(),
                frameRectangleObject["h"].as<float>()
            };

            const auto& sourceSizeObject = frameObject["sourceSize"];

            const math::Size<float, 2> sourceSize{
                sourceSizeObject["w"].as<float>(),
                sourceSizeObject["h"].as<float>()
            };

            const auto& spriteSourceSizeObject = frameObject["spriteSourceSize"];

            const math::Vector<float, 2> sourceOffset{
                spriteSourceSizeObject["x"].as<float>(),
                spriteSourceSizeObject["y"].as<float>()
            };

            const auto& pivotObject = frameObject["pivot"];

            const math::Vector<float, 2> pivot{
                pivotObject["x"].as<float>(),
                pivotObject["y"].as<float>()
            };

            if (frameObject.hasMember("vertices") &&
                frameObject.hasMember("verticesUV") &&
                frameObject.hasMember("triangles"))
            {
                std::vector<std::uint16_t> indices;

                const auto& trianglesObject = frameObject["triangles"];

                for (const auto& triangleObject : trianglesObject)
                    for (const auto& indexObject : triangleObject)
                        indices.push_back(static_cast<std::uint16_t>(indexObject.as<std::uint32_t>()));

                // reverse the vertices, so that they are counterclockwise
                std::reverse(indices.begin(), indices.end());

                std::vector<graphics::Vertex> vertices;

                const auto& verticesObject = frameObject["vertices"];
                const auto& verticesUVObject = frameObject["verticesUV"];

                const math::Vector<float, 2> finalOffset{
                    -sourceSize.v[0] * pivot.v[0] + sourceOffset.v[0],
                    -sourceSize.v[1] * pivot.v[1] + (sourceSize.v[1] - frameRectangle.size.v[1] - sourceOffset.v[1])
                };

                for (std::size_t vertexIndex = 0; vertexIndex < verticesObject.getSize(); ++vertexIndex)
                {
                    const auto& vertexObject = verticesObject[vertexIndex];
                    const auto& vertexUVObject = verticesUVObject[vertexIndex];

                    vertices.emplace_back(math::Vector<float, 3>{static_cast<float>(vertexObject[0].as<std::int32_t>()) + finalOffset.v[0],
                                                           -static_cast<float>(vertexObject[1].as<std::int32_t>()) - finalOffset.v[1], 0.0F},
                                          math::whiteColor,
                                          math::Vector<float, 2>{static_cast<float>(vertexUVObject[0].as<std::int32_t>()) / textureSize.v[0],
                                                           static_cast<float>(vertexUVObject[1].as<std::int32_t>()) / textureSize.v[1]},
                                          math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
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

#endif // OUZEL_ASSETS_SPRITELOADER_HPP
