// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iterator>
#include <string>
#include "LoaderMTL.hpp"
#include "Cache.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

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

        static bool skipWhitespaces(const std::vector<uint8_t>& str,
                                    std::vector<uint8_t>::const_iterator& iterator)
        {
            if (iterator == str.end()) return false;

            for (;;)
            {
                if (iterator == str.end()) break;

                if (isWhitespace(*iterator))
                    ++iterator;
                else
                    break;
            }

            return true;
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

        static bool parseString(const std::vector<uint8_t>& str,
                                std::vector<uint8_t>::const_iterator& iterator,
                                std::string& result)
        {
            result.clear();

            for (;;)
            {
                if (iterator == str.end() || isControlChar(*iterator) || isWhitespace(*iterator)) break;

                result.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            return !result.empty();
        }

        static bool parseFloat(const std::vector<uint8_t>& str,
                               std::vector<uint8_t>::const_iterator& iterator,
                               float& result)
        {
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

            return true;
        }

        LoaderMTL::LoaderMTL():
            Loader(TYPE, {"mtl"})
        {
        }

        bool LoaderMTL::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
            std::string name = filename;
            std::shared_ptr<graphics::Texture> diffuseTexture;
            std::shared_ptr<graphics::Texture> ambientTexture;
            Color diffuseColor = Color::WHITE;
            float opacity = 1.0F;

            uint32_t materialCount = 0;

            std::vector<uint8_t>::const_iterator iterator = data.begin();

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
                    if (!skipWhitespaces(data, iterator) ||
                        !parseString(data, iterator, keyword))
                    {
                        Log(Log::Level::ERR) << "Failed to parse keyword";
                        return false;
                    }

                    if (keyword == "newmtl")
                    {
                        if (materialCount)
                        {
                            std::shared_ptr<graphics::Material> material = std::make_shared<graphics::Material>();
                            material->blendState = engine->getCache()->getBlendState(graphics::BLEND_ALPHA);
                            material->shader = engine->getCache()->getShader(graphics::SHADER_TEXTURE);
                            material->textures[0] = diffuseTexture;
                            material->textures[1] = ambientTexture;
                            material->diffuseColor = diffuseColor;
                            material->opacity = opacity;

                            engine->getCache()->setMaterial(name, material);
                        }

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, name))
                        {
                            Log(Log::Level::ERR) << "Failed to parse material name";
                            return false;
                        }

                        skipLine(data, iterator);

                        diffuseTexture.reset();
                        ambientTexture.reset();
                        diffuseColor = Color::WHITE;
                        opacity = 1.0F;
                    }
                    else if (keyword == "map_Ka") // ambient texture map
                    {
                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, value))
                        {
                            Log(Log::Level::ERR) << "Failed to parse ambient texture map";
                            return false;
                        }

                        skipLine(data, iterator);

                        ambientTexture = engine->getCache()->getTexture(value, mipmaps);
                    }
                    else if (keyword == "map_Kd") // diffuse texture map
                    {
                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, value))
                        {
                            Log(Log::Level::ERR) << "Failed to parse diffuse texture map";
                            return false;
                        }

                        skipLine(data, iterator);

                        diffuseTexture = engine->getCache()->getTexture(value, mipmaps);
                    }
                    else if (keyword == "Ka") // ambient color
                        skipLine(data, iterator);
                    else if (keyword == "Kd") // diffuse color
                    {
                        float color[4];

                        if (!skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, color[0]) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, color[1]) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, color[2]))
                        {
                            Log(Log::Level::ERR) << "Failed to parse normal";
                            return false;
                        }

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
                        if (!skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, opacity))
                        {
                            Log(Log::Level::ERR) << "Failed to parse opacity";
                            return false;
                        }

                        skipLine(data, iterator);
                    }
                    else if (keyword == "Tr") // transparency
                    {
                        float transparency;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, transparency))
                        {
                            Log(Log::Level::ERR) << "Failed to parse transparency";
                            return false;
                        }

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
                material->blendState = engine->getCache()->getBlendState(graphics::BLEND_ALPHA);
                material->shader = engine->getCache()->getShader(graphics::SHADER_TEXTURE);
                material->textures[0] = diffuseTexture;
                material->textures[1] = ambientTexture;
                material->diffuseColor = diffuseColor;
                material->opacity = opacity;

                engine->getCache()->setMaterial(name, material);
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
