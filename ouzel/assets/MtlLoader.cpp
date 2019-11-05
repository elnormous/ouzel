// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <iterator>
#include <stdexcept>
#include <string>
#include "MtlLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace assets
    {
        namespace
        {
            constexpr auto isWhitespace(uint8_t c)
            {
                return c == ' ' || c == '\t';
            }

            constexpr auto isNewline(uint8_t c)
            {
                return c == '\r' || c == '\n';
            }

            constexpr auto isControlChar(uint8_t c)
            {
                return c <= 0x1F;
            }

            void skipWhitespaces(std::vector<uint8_t>::const_iterator& iterator,
                                 std::vector<uint8_t>::const_iterator end)
            {
                while (iterator != end)
                    if (isWhitespace(*iterator))
                        ++iterator;
                    else
                        break;
            }

            void skipLine(std::vector<uint8_t>::const_iterator& iterator,
                          std::vector<uint8_t>::const_iterator end)
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

            std::string parseString(std::vector<uint8_t>::const_iterator& iterator,
                                    std::vector<uint8_t>::const_iterator end)
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

            float parseFloat(std::vector<uint8_t>::const_iterator& iterator,
                             std::vector<uint8_t>::const_iterator end)
            {
                std::string value;
                uint32_t length = 1;

                if (iterator != end && *iterator == '-')
                {
                    value.push_back(static_cast<char>(*iterator));
                    ++length;
                    ++iterator;
                }

                while (iterator != end && *iterator >= '0' && *iterator <= '9')
                {
                    value.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }

                if (iterator != end && *iterator == '.')
                {
                    value.push_back(static_cast<char>(*iterator));
                    ++length;
                    ++iterator;

                    while (iterator != end && *iterator >= '0' && *iterator <= '9')
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
            Loader(initCache, Loader::Material)
        {
        }

        bool MtlLoader::loadAsset(Bundle& bundle,
                                  const std::string& name,
                                  const std::vector<uint8_t>& data,
                                  bool mipmaps)
        {
            std::string materialName = name;
            std::shared_ptr<graphics::Texture> diffuseTexture;
            std::shared_ptr<graphics::Texture> ambientTexture;
            Color diffuseColor = Color::white();
            float opacity = 1.0F;

            uint32_t materialCount = 0;

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
                else if (*iterator == '#')
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
                            material->blendState = cache.getBlendState(BLEND_ALPHA);
                            material->shader = cache.getShader(SHADER_TEXTURE);
                            material->textures[0] = diffuseTexture;
                            material->textures[1] = ambientTexture;
                            material->diffuseColor = diffuseColor;
                            material->opacity = opacity;
                            material->cullMode = graphics::CullMode::Back;

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
                            bundle.loadAsset(Loader::Image, value, value, mipmaps);
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
                        float transparency;

                        skipWhitespaces(iterator, data.end());
                        transparency = parseFloat(iterator, data.end());

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
                material->blendState = cache.getBlendState(BLEND_ALPHA);
                material->shader = cache.getShader(SHADER_TEXTURE);
                material->textures[0] = diffuseTexture;
                material->textures[1] = ambientTexture;
                material->diffuseColor = diffuseColor;
                material->opacity = opacity;
                material->cullMode = graphics::CullMode::Back;

                bundle.setMaterial(materialName, std::move(material));
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
