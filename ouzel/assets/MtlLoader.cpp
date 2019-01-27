// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
        static bool isWhitespace(uint8_t c)
        {
            return c == ' ' || c == '\t';
        }

        static bool isNewline(uint8_t c)
        {
            return c == '\r' || c == '\n';
        }

        static bool isControlChar(uint8_t c)
        {
            return c <= 0x1F;
        }

        static void skipWhitespaces(const std::vector<uint8_t>& str,
                                    std::vector<uint8_t>::const_iterator& iterator)
        {
            for (;;)
            {
                if (iterator == str.end()) break;

                if (isWhitespace(*iterator))
                    ++iterator;
                else
                    break;
            }
        }

        static void skipLine(const std::vector<uint8_t>& str,
                             std::vector<uint8_t>::const_iterator& iterator)
        {
            for (;;)
            {
                if (iterator == str.end()) break;

                if (isNewline(*iterator))
                {
                    ++iterator;
                    break;
                }

                ++iterator;
            }
        }

        static std::string parseString(const std::vector<uint8_t>& str,
                                       std::vector<uint8_t>::const_iterator& iterator)
        {
            std::string result;

            for (;;)
            {
                if (iterator == str.end() || isControlChar(*iterator) || isWhitespace(*iterator)) break;

                result.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            if (result.empty())
                throw std::runtime_error("Invalid string");

            return result;
        }

        static float parseFloat(const std::vector<uint8_t>& str,
                               std::vector<uint8_t>::const_iterator& iterator)
        {
            float result;
            std::string value;
            uint32_t length = 1;

            if (iterator != str.end() && *iterator == '-')
            {
                value.push_back(static_cast<char>(*iterator));
                ++length;
                ++iterator;
            }

            for (;;)
            {
                if (iterator == str.end() || *iterator < '0' || *iterator > '9') break;

                value.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            if (iterator != str.end() && *iterator == '.')
            {
                value.push_back(static_cast<char>(*iterator));
                ++length;
                ++iterator;

                for (;;)
                {
                    if (iterator == str.end() || *iterator < '0' || *iterator > '9') break;

                    value.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }
            }

            if (value.length() < length) return false;

            result = std::stof(value);

            return result;
        }

        MtlLoader::MtlLoader(Cache& initCache):
            Loader(initCache, TYPE)
        {
        }

        bool MtlLoader::loadAsset(Bundle& bundle, const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            std::string name = filename;
            std::shared_ptr<graphics::Texture> diffuseTexture;
            std::shared_ptr<graphics::Texture> ambientTexture;
            Color diffuseColor = Color::WHITE;
            float opacity = 1.0F;

            uint32_t materialCount = 0;

            auto iterator = data.cbegin();

            std::string keyword;
            std::string value;

            for (;;)
            {
                if (iterator == data.end()) break;

                if (isNewline(*iterator))
                {
                    // skip empty lines
                    ++iterator;
                }
                else if (*iterator == '#')
                {
                    // skip the comment
                    skipLine(data, iterator);
                }
                else
                {
                    skipWhitespaces(data, iterator);
                    keyword = parseString(data, iterator);

                    if (keyword == "newmtl")
                    {
                        if (materialCount)
                        {
                            std::shared_ptr<graphics::Material> material = std::make_shared<graphics::Material>();
                            material->blendState = cache.getBlendState(BLEND_ALPHA);
                            material->shader = cache.getShader(SHADER_TEXTURE);
                            material->textures[0] = diffuseTexture;
                            material->textures[1] = ambientTexture;
                            material->diffuseColor = diffuseColor;
                            material->opacity = opacity;
                            material->cullMode = graphics::CullMode::BACK;

                            bundle.setMaterial(name, material);
                        }

                        skipWhitespaces(data, iterator);
                        name = parseString(data, iterator);

                        skipLine(data, iterator);

                        diffuseTexture.reset();
                        ambientTexture.reset();
                        diffuseColor = Color::WHITE;
                        opacity = 1.0F;
                    }
                    else if (keyword == "map_Ka") // ambient texture map
                    {
                        skipWhitespaces(data, iterator);
                        value = parseString(data, iterator);

                        skipLine(data, iterator);

                        ambientTexture = cache.getTexture(value);
                    }
                    else if (keyword == "map_Kd") // diffuse texture map
                    {
                        skipWhitespaces(data, iterator);
                        value = parseString(data, iterator);

                        skipLine(data, iterator);

                        diffuseTexture = cache.getTexture(value);

                        if (!diffuseTexture)
                        {
                            bundle.loadAsset(Loader::IMAGE, value, mipmaps);
                            diffuseTexture = cache.getTexture(value);
                        }
                    }
                    else if (keyword == "Ka") // ambient color
                        skipLine(data, iterator);
                    else if (keyword == "Kd") // diffuse color
                    {
                        float color[4];

                        skipWhitespaces(data, iterator);
                        color[0] = parseFloat(data, iterator);
                        skipWhitespaces(data, iterator);
                        color[1] = parseFloat(data, iterator);
                        skipWhitespaces(data, iterator);
                        color[2] = parseFloat(data, iterator);

                        skipLine(data, iterator);

                        color[3] = 1.0F;
                        diffuseColor = Color(color);
                    }
                    else if (keyword == "Ks") // specular color
                        skipLine(data, iterator);
                    else if (keyword == "Ke") // emissive color
                        skipLine(data, iterator);
                    else if (keyword == "d") // opacity
                    {
                        skipWhitespaces(data, iterator);
                        opacity = parseFloat(data, iterator);

                        skipLine(data, iterator);
                    }
                    else if (keyword == "Tr") // transparency
                    {
                        float transparency;

                        skipWhitespaces(data, iterator);
                        transparency = parseFloat(data, iterator);

                        skipLine(data, iterator);

                        // d = 1 - Tr
                        opacity = 1.0F - transparency;
                    }
                    else
                    {
                        // skip all unknown commands
                        skipLine(data, iterator);
                    }

                    if (!materialCount) ++materialCount; // if we got at least one attribute, we have an material
                }
            }

            if (materialCount)
            {
                std::shared_ptr<graphics::Material> material = std::make_shared<graphics::Material>();
                material->blendState = cache.getBlendState(BLEND_ALPHA);
                material->shader = cache.getShader(SHADER_TEXTURE);
                material->textures[0] = diffuseTexture;
                material->textures[1] = ambientTexture;
                material->diffuseColor = diffuseColor;
                material->opacity = opacity;
                material->cullMode = graphics::CullMode::BACK;

                bundle.setMaterial(name, material);
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
