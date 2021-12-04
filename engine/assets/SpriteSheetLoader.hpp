// Ouzel by Elviss Strazdins

#ifndef OUZEL_ASSETS_SPRITESHEETLOADER_HPP
#define OUZEL_ASSETS_SPRITESHEETLOADER_HPP

#include <string>
#include <vector>
#include "Bundle.hpp"
#include "../scene/SpriteRenderer.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    inline bool loadSpriteSheet(Cache&,
                                Bundle& bundle,
                                const std::string& name,
                                const std::vector<std::byte>& data,
                                const Asset::Options& options)
    {
        const auto d = json::parse(data);

        if (!d.hasMember("filename") ||
            !d.hasMember("spritesX") ||
            !d.hasMember("spritesY") ||
            !d.hasMember("pivotX") ||
            !d.hasMember("pivotY"))
            return false;

        const auto& imageFilename = d["filename"].as<std::string>();
        const auto spritesX = d["spritesX"].as<std::size_t>();
        const auto spritesY = d["spritesY"].as<std::size_t>();
        const math::Vector<float, 2> pivot{
            d["pivotX"].as<float>(),
            d["pivotY"].as<float>()
        };

        scene::SpriteData spriteData;

        spriteData.texture = bundle.getTexture(imageFilename);
        if (!spriteData.texture)
        {
            bundle.loadAsset(Asset::Type::image, imageFilename, imageFilename, options);
            spriteData.texture = bundle.getTexture(imageFilename);
        }

        const math::Size<float, 2> textureSize{
            static_cast<float>(spriteData.texture->getSize().v[0]),
            static_cast<float>(spriteData.texture->getSize().v[1])
        };

        if (spriteData.texture)
        {
            const auto spriteSize = math::Size<float, 2>{
                textureSize.v[0] / static_cast<float>(spritesX),
                textureSize.v[1] / static_cast<float>(spritesY)
            };

            scene::SpriteData::Animation animation;
            animation.frames.reserve(spritesX * spritesY);

            for (std::size_t x = 0; x < spritesX; ++x)
                for (std::size_t y = 0; y < spritesY; ++y)
                {
                    const math::Rect<float> rectangle{
                        spriteSize.v[0] * static_cast<float>(x),
                        spriteSize.v[1] * static_cast<float>(y),
                        spriteSize.v[0],
                        spriteSize.v[1]
                    };

                    animation.frames.emplace_back(imageFilename, textureSize, rectangle, false, spriteSize, math::Vector<float, 2>{}, pivot);
                }

            spriteData.animations[""] = std::move(animation);

            bundle.setSpriteData(name, spriteData);
        }

        return true;
    }
}

#endif // OUZEL_ASSETS_SPRITESHEETLOADER_HPP
