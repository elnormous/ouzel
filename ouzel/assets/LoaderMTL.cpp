// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <iterator>
#include <sstream>
#include "LoaderMTL.hpp"
#include "Cache.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderMTL::LoaderMTL():
            Loader({"mtl"})
        {
        }

        bool LoaderMTL::loadAsset(const std::string& filename, const std::vector<uint8_t>& data)
        {
            std::stringstream stream;
            std::copy(data.begin(), data.end(), std::ostream_iterator<uint8_t>(stream));

            std::string name = filename;
            std::shared_ptr<graphics::Texture> diffuseTexture;
            std::shared_ptr<graphics::Texture> ambientTexture;
            Color diffuseColor = Color::WHITE;
            float opacity = 1.0f;

            uint32_t materialCount = 0;
            std::string read;

            for (std::string line; std::getline(stream, line);)
            {
                if (!line.empty())
                {
                    if (line[0] == '#') continue; // comment

                    std::stringstream lineStream;
                    lineStream << line;

                    lineStream >> read;

                    if (read == "newmtl")
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

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse material name";
                            return false;
                        }

                        lineStream >> read;

                        name = read;
                        diffuseTexture.reset();
                        ambientTexture.reset();
                        diffuseColor = Color::WHITE;
                        opacity = 1.0f;
                    }
                    else if (read == "map_Ka") // ambient texture map
                    {
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse ambient texture file name";
                            return false;
                        }

                        lineStream >> read;

                        ambientTexture = engine->getCache()->getTexture(read);
                    }
                    else if (read == "map_Kd") // diffuse texture map
                    {
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse diffuse texture file name";
                            return false;
                        }

                        lineStream >> read;

                        diffuseTexture = engine->getCache()->getTexture(read);
                    }
                    else if (read == "Ka") // ambient color
                    {

                    }
                    else if (read == "Kd") // diffuse color
                    {
                        float color[4];
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse diffuse color";
                            return false;
                        }

                        lineStream >> color[0];

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse diffuse color";
                            return false;
                        }

                        lineStream >> color[1];

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse diffuse color";
                            return false;
                        }

                        lineStream >> color[2];
                        color[3] = 1.0f;

                        diffuseColor = Color(color);
                    }
                    else if (read == "Ks") // specular color
                    {

                    }
                    else if (read == "Ke") // emissive color
                    {

                    }
                    else if (read == "d") // opacity
                    {
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse opacity";
                            return false;
                        }

                        lineStream >> opacity;
                    }
                    else if (read == "Tr") // transparency
                    {
                        float transparency;

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse transparency";
                            return false;
                        }

                        lineStream >> transparency;

                        // d = 1 - Tr
                        opacity = 1.0f - transparency;
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
