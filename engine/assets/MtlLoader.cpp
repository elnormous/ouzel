// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include <string>
#include "MtlLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../core/Engine.hpp"

namespace ouzel::assets
{
    namespace
    {
        constexpr auto isWhitespace(std::byte c)
        {
            return static_cast<char>(c) == ' ' ||
                static_cast<char>(c) == '\t';
        }

        constexpr auto isNewline(std::byte c)
        {
            return static_cast<char>(c) == '\r' ||
                static_cast<char>(c) == '\n';
        }

        constexpr auto isControlChar(std::byte c)
        {
            return static_cast<std::uint8_t>(c) <= 0x1F;
        }

        void skipWhitespaces(std::vector<std::byte>::const_iterator& iterator,
                             std::vector<std::byte>::const_iterator end)
        {
            while (iterator != end)
                if (isWhitespace(*iterator))
                    ++iterator;
                else
                    break;
        }

        void skipLine(std::vector<std::byte>::const_iterator& iterator,
                      std::vector<std::byte>::const_iterator end)
        {
            while (iterator != end)
            {
                if (isNewline(*iterator))
                {
                    ++iterator;
                    break;
                }

                ++iterator;
            }
        }

        std::string parseString(std::vector<std::byte>::const_iterator& iterator,
                                std::vector<std::byte>::const_iterator end)
        {
            std::string result;

            while (iterator != end && !isControlChar(*iterator) && !isWhitespace(*iterator))
            {
                result.push_back(static_cast<char>(*iterator));
                ++iterator;
            }

            if (result.empty())
                throw std::runtime_error("Invalid string");

            return result;
        }

        float parseFloat(std::vector<std::byte>::const_iterator& iterator,
                         std::vector<std::byte>::const_iterator end)
        {
            std::string value;
            std::uint32_t length = 1;

            if (iterator != end &&
                static_cast<char>(*iterator) == '-')
            {
                value.push_back(static_cast<char>(*iterator));
                ++length;
                ++iterator;
            }

            while (iterator != end &&
                   static_cast<char>(*iterator) >= '0' &&
                   static_cast<char>(*iterator) <= '9')
            {
                value.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            if (iterator != end &&
                static_cast<char>(*iterator) == '.')
            {
                value.push_back(static_cast<char>(*iterator));
                ++length;
                ++iterator;

                while (iterator != end &&
                       static_cast<char>(*iterator) >= '0' &&
                       static_cast<char>(*iterator) <= '9')
                {
                    value.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }
            }

            // parse exponent
            if (iterator != end &&
                (static_cast<char>(*iterator) == 'e' ||
                 static_cast<char>(*iterator) == 'E'))
            {
                value.push_back(static_cast<char>(*iterator));
                if (++iterator == end)
                    throw std::runtime_error("Invalid exponent");

                if (static_cast<char>(*iterator) == '+' ||
                    static_cast<char>(*iterator) == '-')
                    value.push_back(static_cast<char>(*iterator++));

                if (iterator == end ||
                    static_cast<char>(*iterator) < '0' ||
                    static_cast<char>(*iterator) > '9')
                    throw std::runtime_error("Invalid exponent");

                while (iterator != end &&
                       static_cast<char>(*iterator) >= '0' &&
                       static_cast<char>(*iterator) <= '9')
                {
                    value.push_back(static_cast<char>(*iterator));
                    ++iterator;
                }
            }

            if (value.length() < length) return false;

            return std::stof(value);
        }
    }

    MtlLoader::MtlLoader(Cache& initCache):
        Loader(initCache, Type::material)
    {
    }

    bool MtlLoader::loadAsset(Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
                              bool mipmaps)
    {
        std::string materialName = name;
        std::shared_ptr<graphics::Texture> diffuseTexture;
        std::shared_ptr<graphics::Texture> ambientTexture;
        Color diffuseColor = Color::white();
        float opacity = 1.0F;

        std::uint32_t materialCount = 0;

        auto iterator = data.cbegin();

        std::string keyword;
        std::string value;

        while (iterator != data.end())
        {
            if (isNewline(*iterator))
            {
                // skip empty lines
                ++iterator;
            }
            else if (static_cast<char>(*iterator) == '#')
            {
                // skip the comment
                skipLine(iterator, data.end());
            }
            else
            {
                skipWhitespaces(iterator, data.end());
                keyword = parseString(iterator, data.end());

                if (keyword == "newmtl")
                {
                    if (materialCount)
                    {
                        auto material = std::make_unique<graphics::Material>();
                        material->blendState = cache.getBlendState(blendAlpha);
                        material->shader = diffuseTexture ? cache.getShader(shaderTexture) : cache.getShader(shaderColor);
                        material->textures[0] = diffuseTexture;
                        material->textures[1] = ambientTexture;
                        material->diffuseColor = diffuseColor;
                        material->opacity = opacity;
                        material->cullMode = graphics::CullMode::back;

                        bundle.setMaterial(materialName, std::move(material));
                    }

                    skipWhitespaces(iterator, data.end());
                    materialName = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    diffuseTexture.reset();
                    ambientTexture.reset();
                    diffuseColor = Color::white();
                    opacity = 1.0F;
                }
                else if (keyword == "map_Ka") // ambient texture map
                {
                    skipWhitespaces(iterator, data.end());
                    value = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    ambientTexture = cache.getTexture(value);
                }
                else if (keyword == "map_Kd") // diffuse texture map
                {
                    skipWhitespaces(iterator, data.end());
                    value = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    diffuseTexture = cache.getTexture(value);

                    if (!diffuseTexture)
                    {
                        bundle.loadAsset(Type::image, value, value, mipmaps);
                        diffuseTexture = cache.getTexture(value);
                    }
                }
                else if (keyword == "Ka") // ambient color
                    skipLine(iterator, data.end());
                else if (keyword == "Kd") // diffuse color
                {
                    float color[4];

                    skipWhitespaces(iterator, data.end());
                    color[0] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    color[1] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    color[2] = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    color[3] = 1.0F;
                    diffuseColor = Color(color);
                }
                else if (keyword == "Ks") // specular color
                    skipLine(iterator, data.end());
                else if (keyword == "Ke") // emissive color
                    skipLine(iterator, data.end());
                else if (keyword == "d") // opacity
                {
                    skipWhitespaces(iterator, data.end());
                    opacity = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());
                }
                else if (keyword == "Tr") // transparency
                {
                    skipWhitespaces(iterator, data.end());
                    const float transparency = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    // d = 1 - Tr
                    opacity = 1.0F - transparency;
                }
                else
                {
                    // skip all unknown commands
                    skipLine(iterator, data.end());
                }

                if (!materialCount) ++materialCount; // if we got at least one attribute, we have an material
            }
        }

        if (materialCount)
        {
            auto material = std::make_unique<graphics::Material>();
            material->blendState = cache.getBlendState(blendAlpha);
            material->shader = cache.getShader(shaderTexture);
            material->textures[0] = diffuseTexture;
            material->textures[1] = ambientTexture;
            material->diffuseColor = diffuseColor;
            material->opacity = opacity;
            material->cullMode = graphics::CullMode::back;

            bundle.setMaterial(materialName, std::move(material));
        }

        return true;
    }
}
