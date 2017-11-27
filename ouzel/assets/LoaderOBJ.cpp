// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <iterator>
#include <map>
#include <sstream>
#include <tuple>
#include "LoaderOBJ.hpp"
#include "Cache.hpp"
#include "core/Engine.hpp"
#include "graphics/Material.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace assets
    {
        LoaderOBJ::LoaderOBJ():
            Loader({"obj"})
        {
        }

        bool LoaderOBJ::loadAsset(const std::string& filename, const std::vector<uint8_t>& data)
        {
            std::stringstream stream;
            std::copy(data.begin(), data.end(), std::ostream_iterator<uint8_t>(stream));

            std::string name = filename;
            std::shared_ptr<graphics::Material> material;
            std::vector<Vector3> positions;
            std::vector<Vector2> texCoords;
            std::vector<Vector3> normals;
            std::vector<graphics::Vertex> vertices;
            std::map<std::tuple<uint32_t, uint32_t, uint32_t>, uint32_t> vertexMap;
            std::vector<uint32_t> indices;
            Box3 boundingBox;

            uint32_t objectCount = 0;

            for (std::string line; std::getline(stream, line);)
            {
                if (!line.empty())
                {
                    if (line[0] == '#') continue; // comment

                    std::stringstream lineStream;
                    lineStream << line;

                    std::string read;
                    lineStream >> read;

                    if (read == "mtllib")
                    {
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse material library file name";
                            return false;
                        }

                        lineStream >> read;
                        cache->loadAsset(read);
                    }
                    else if (read == "usemtl")
                    {
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse material name";
                            return false;
                        }

                        lineStream >> read;
                        material = cache->getMaterial(read);
                    }
                    else if (read == "o") // object name
                    {
                        if (objectCount)
                        {
                            scene::ModelData modelData;
                            modelData.init(boundingBox, indices, vertices, material);
                            engine->getCache()->setModelData(name, modelData);
                        }

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse object name";
                            return false;
                        }

                        lineStream >> read;

                        name = read;
                        material.reset();
                        positions.clear();
                        texCoords.clear();
                        normals.clear();
                        vertices.clear();
                        indices.clear();
                        boundingBox.reset();
                        ++objectCount;
                    }
                    else if (read == "v") // vertex
                    {
                        Vector3 position;
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse position";
                            return false;
                        }

                        lineStream >> position.x;

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse position";
                            return false;
                        }

                        lineStream >> position.y;

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse position";
                            return false;
                        }

                        lineStream >> position.z;

                        positions.push_back(position);
                    }
                    else if (read == "vt") // texture coordinates
                    {
                        Vector3 texCoord;
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse texture coordinates";
                            return false;
                        }

                        lineStream >> texCoord.x;

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse texture coordinates";
                            return false;
                        }

                        lineStream >> texCoord.y;
                        
                        texCoords.push_back(texCoord);
                    }
                    else if (read == "vn") // normal
                    {
                        Vector3 normal;
                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse normal";
                            return false;
                        }

                        lineStream >> normal.x;

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse normal";
                            return false;
                        }

                        lineStream >> normal.y;

                        if (lineStream.eof())
                        {
                            Log(Log::Level::ERR) << "Failed to parse normal";
                            return false;
                        }

                        lineStream >> normal.z;
                        
                        normals.push_back(normal);
                    }
                    else if (read == "f") // face
                    {
                        std::vector<uint32_t> vertexIndices;

                        while (!lineStream.eof())
                        {
                            lineStream >> read;

                            std::stringstream vertexStream;
                            vertexStream << read;

                            std::tuple<uint32_t, uint32_t, uint32_t> i = std::make_tuple(0, 0, 0);

                            if (!std::getline(vertexStream, read, '/'))
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
                            }

                            long index = std::strtol(read.c_str(), nullptr, 0);

                            if (index < 0)
                                std::get<0>(i) = static_cast<uint32_t>(static_cast<long>(positions.size()) + index);
                            else if (index > 0)
                                std::get<0>(i) = static_cast<uint32_t>(index - 1);
                            else
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
                            }

                            if (std::getline(vertexStream, read, '/'))
                            {
                                index = std::strtol(read.c_str(), nullptr, 0);

                                if (index < 0)
                                    std::get<1>(i) = static_cast<uint32_t>(static_cast<long>(texCoords.size()) + index);
                                else if (index > 0)
                                    std::get<1>(i) = static_cast<uint32_t>(index - 1);
                                else
                                {
                                    Log(Log::Level::ERR) << "Failed to parse face";
                                    return false;
                                }
                            }

                            if (std::getline(vertexStream, read, '/'))
                            {
                                index = std::strtol(read.c_str(), nullptr, 0);

                                if (index < 0)
                                    std::get<2>(i) = static_cast<uint32_t>(static_cast<long>(normals.size()) + index);
                                else if (index > 0)
                                    std::get<2>(i) = static_cast<uint32_t>(index - 1);
                                else
                                {
                                    Log(Log::Level::ERR) << "Failed to parse face";
                                    return false;
                                }
                            }

                            auto vertexIterator = vertexMap.find(i);
                            if (vertexIterator != vertexMap.end())
                            {
                                vertexIndices.push_back(vertexIterator->second);
                            }
                            else
                            {
                                graphics::Vertex vertex;
                                vertex.position = positions[std::get<0>(i)];
                                vertex.texCoords[0] = texCoords[std::get<1>(i)];
                                vertex.color = Color::WHITE;
                                vertex.normal = normals[std::get<2>(i)];
                                vertexIndices.push_back(static_cast<uint32_t>(vertices.size()));
                                vertices.push_back(vertex);
                                boundingBox.insertPoint(vertex.position);
                            }
                        }

                        if (vertexIndices.size() < 3)
                        {
                            Log(Log::Level::ERR) << "Invalid face count";
                            return false;
                        }
                        else if (vertexIndices.size() == 3)
                        {
                            for (uint32_t vertexIndex : vertexIndices)
                            {
                                indices.push_back(vertexIndex);
                            }
                        }
                        else
                        {
                            // TODO: implement
                        }
                    }

                    if (!objectCount) ++objectCount; // if we got at least one attribute, we have an object
                }
            }

            if (objectCount)
            {
                scene::ModelData modelData;
                modelData.init(boundingBox, indices, vertices, material);
                engine->getCache()->setModelData(name, modelData);
            }

            return true;
        }
    } // namespace assets
} // namespace ouzel
