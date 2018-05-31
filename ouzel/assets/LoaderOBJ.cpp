// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <iterator>
#include <map>
#include <string>
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

        static bool parseInt32(const std::vector<uint8_t>& str,
                               std::vector<uint8_t>::const_iterator& iterator,
                               int32_t& result)
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

            if (value.length() < length) return false;

            result = std::stoi(value);

            return true;
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

        static bool parseToken(const std::vector<uint8_t>& str,
                               std::vector<uint8_t>::const_iterator& iterator,
                               char token)
        {
            if (iterator == str.end() || *iterator != static_cast<uint8_t>(token)) return false;

            ++iterator;

            return true;
        }

        LoaderOBJ::LoaderOBJ():
            Loader(TYPE, {"obj"})
        {
        }

        bool LoaderOBJ::loadAsset(const std::string& filename, const std::vector<uint8_t>& data, bool mipmaps)
        {
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

                    if (keyword == "mtllib")
                    {
                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, value))
                        {
                            Log(Log::Level::ERR) << "Failed to parse material library";
                            return false;
                        }

                        skipLine(data, iterator);

                        cache->loadAsset(value, mipmaps);
                    }
                    else if (keyword == "usemtl")
                    {
                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, value))
                        {
                            Log(Log::Level::ERR) << "Failed to parse material name";
                            return false;
                        }

                        skipLine(data, iterator);

                        material = cache->getMaterial(value);
                    }
                    else if (keyword == "o")
                    {
                        if (objectCount)
                        {
                            scene::ModelData modelData;
                            modelData.init(boundingBox, indices, vertices, material);
                            engine->getCache()->setModelData(name, modelData);
                        }

                        if (!skipWhitespaces(data, iterator) ||
                            !parseString(data, iterator, name))
                        {
                            Log(Log::Level::ERR) << "Failed to parse object name";
                            return false;
                        }

                        skipLine(data, iterator);

                        material.reset();
                        positions.clear();
                        texCoords.clear();
                        normals.clear();
                        vertices.clear();
                        indices.clear();
                        boundingBox.reset();
                        ++objectCount;
                    }
                    else if (keyword == "v")
                    {
                        Vector3 position;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, position.x) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, position.y) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, position.z))
                        {
                            Log(Log::Level::ERR) << "Failed to parse position";
                            return false;
                        }

                        skipLine(data, iterator);

                        positions.push_back(position);
                    }
                    else if (keyword == "vt")
                    {
                        Vector3 texCoord;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, texCoord.x) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, texCoord.y))
                        {
                            Log(Log::Level::ERR) << "Failed to parse texture coordinates";
                            return false;
                        }

                        skipLine(data, iterator);

                        texCoords.push_back(texCoord);
                    }
                    else if (keyword == "vn")
                    {
                        Vector3 normal;

                        if (!skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, normal.x) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, normal.y) ||
                            !skipWhitespaces(data, iterator) ||
                            !parseFloat(data, iterator, normal.z))
                        {
                            Log(Log::Level::ERR) << "Failed to parse normal";
                            return false;
                        }

                        skipLine(data, iterator);

                        normals.push_back(normal);
                    }
                    else if (keyword == "f")
                    {
                        std::vector<uint32_t> vertexIndices;

                        std::tuple<uint32_t, uint32_t, uint32_t> i = std::make_tuple(0, 0, 0);
                        int32_t positionIndex = 0, texCoordIndex = 0, normalIndex = 0;

                        for (;;)
                        {
                            if (iterator == data.end() || isNewline(*iterator)) break;

                            if (!skipWhitespaces(data, iterator))
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
                            }

                            if (!parseInt32(data, iterator, positionIndex) ||
                                !parseToken(data, iterator, '/') ||
                                !parseInt32(data, iterator, texCoordIndex) ||
                                !parseToken(data, iterator, '/') ||
                                !parseInt32(data, iterator, normalIndex))
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
                            }

                            if (positionIndex < 0)
                                std::get<0>(i) = static_cast<uint32_t>(static_cast<long>(positions.size()) + positionIndex);
                            else if (positionIndex > 0)
                                std::get<0>(i) = static_cast<uint32_t>(positionIndex - 1);
                            else
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
                            }

                            if (texCoordIndex < 0)
                                std::get<1>(i) = static_cast<uint32_t>(static_cast<long>(texCoords.size()) + texCoordIndex);
                            else if (texCoordIndex > 0)
                                std::get<1>(i) = static_cast<uint32_t>(texCoordIndex - 1);
                            else
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
                            }

                            if (normalIndex < 0)
                                std::get<2>(i) = static_cast<uint32_t>(static_cast<long>(normals.size()) + normalIndex);
                            else if (normalIndex > 0)
                                std::get<2>(i) = static_cast<uint32_t>(normalIndex - 1);
                            else
                            {
                                Log(Log::Level::ERR) << "Failed to parse face";
                                return false;
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
                                indices.push_back(vertexIndex);
                        }
                        else
                        {
                            for (uint32_t index = 0; index < vertexIndices.size() - 2; ++index)
                            {
                                indices.push_back(vertexIndices[0]);
                                indices.push_back(vertexIndices[index + 1]);
                                indices.push_back(vertexIndices[index + 2]);
                            }
                        }
                    }
                    else
                    {
                        // skip all unknown commands
                        skipLine(data, iterator);
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
