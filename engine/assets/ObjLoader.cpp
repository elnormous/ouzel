// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <map>
#include <stdexcept>
#include <string>
#include <tuple>
#include "ObjLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../graphics/Material.hpp"

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

        std::int32_t parseInt32(std::vector<std::byte>::const_iterator& iterator,
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

            if (value.length() < length) return false;

            return std::stoi(value);
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

        bool parseToken(const std::vector<std::byte>& str,
                        std::vector<std::byte>::const_iterator& iterator,
                        char token)
        {
            if (iterator == str.end() || static_cast<char>(*iterator) != token) return false;

            ++iterator;

            return true;
        }
    }

    ObjLoader::ObjLoader(Cache& initCache):
        Loader(initCache, Type::staticMesh)
    {
    }

    bool ObjLoader::loadAsset(Bundle& bundle,
                              const std::string& name,
                              const std::vector<std::byte>& data,
                              bool mipmaps)
    {
        std::string objectName = name;
        const graphics::Material* material = nullptr;
        std::vector<Vector3F> positions;
        std::vector<Vector2F> texCoords;
        std::vector<Vector3F> normals;
        std::vector<graphics::Vertex> vertices;
        std::map<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::uint32_t> vertexMap;
        std::vector<std::uint32_t> indices;
        Box3F boundingBox;

        std::uint32_t objectCount = 0;

        auto iterator = data.cbegin();

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
                const auto keyword = parseString(iterator, data.end());

                if (keyword == "mtllib")
                {
                    skipWhitespaces(iterator, data.end());
                    const auto filename = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    //if (!cache.getMaterial(filename))
                    // TODO don't load material lib every time
                    bundle.loadAsset(Type::material, filename, filename, mipmaps);
                }
                else if (keyword == "usemtl")
                {
                    skipWhitespaces(iterator, data.end());
                    const auto materialName = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    material = cache.getMaterial(materialName);
                }
                else if (keyword == "o")
                {
                    if (objectCount)
                    {
                        scene::StaticMeshData meshData(boundingBox, indices, vertices, material);
                        bundle.setStaticMeshData(objectName, std::move(meshData));
                    }

                    skipWhitespaces(iterator, data.end());
                    objectName = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    material = nullptr;
                    vertices.clear();
                    indices.clear();
                    vertexMap.clear();
                    boundingBox.reset();
                    ++objectCount;
                }
                else if (keyword == "v")
                {
                    Vector3F position;

                    skipWhitespaces(iterator, data.end());
                    position.v[0] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    position.v[1] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    position.v[2] = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    positions.push_back(position);
                }
                else if (keyword == "vt")
                {
                    Vector2F texCoord;

                    skipWhitespaces(iterator, data.end());
                    texCoord.v[0] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    texCoord.v[1] = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    texCoords.push_back(texCoord);
                }
                else if (keyword == "vn")
                {
                    Vector3F normal;

                    skipWhitespaces(iterator, data.end());
                    normal.v[0] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    normal.v[1] = parseFloat(iterator, data.end());
                    skipWhitespaces(iterator, data.end());
                    normal.v[2] = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    normals.push_back(normal);
                }
                else if (keyword == "f")
                {
                    std::vector<std::uint32_t> vertexIndices;

                    auto i = std::make_tuple<std::uint32_t, std::uint32_t, std::uint32_t>(0, 0, 0);
                    std::int32_t positionIndex = 0;
                    std::int32_t texCoordIndex = 0;
                    std::int32_t normalIndex = 0;

                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhitespaces(iterator, data.end());
                        positionIndex = parseInt32(iterator, data.end());

                        if (positionIndex < 0)
                            positionIndex = static_cast<std::int32_t>(positions.size()) + positionIndex + 1;

                        if (positionIndex < 1 || positionIndex > static_cast<std::int32_t>(positions.size()))
                            throw std::runtime_error("Invalid position index");

                        std::get<0>(i) = static_cast<std::uint32_t>(positionIndex);

                        // has texture coordinates
                        if (parseToken(data, iterator, '/'))
                        {
                            // two slashes in a row indicates no texture coordinates
                            if (iterator != data.end() &&
                                static_cast<char>(*iterator) != '/')
                            {
                                texCoordIndex = parseInt32(iterator, data.end());

                                if (texCoordIndex < 0)
                                    texCoordIndex = static_cast<std::int32_t>(texCoords.size()) + texCoordIndex + 1;

                                if (texCoordIndex < 1 || texCoordIndex > static_cast<std::int32_t>(texCoords.size()))
                                    throw std::runtime_error("Invalid texture coordinate index");

                                std::get<1>(i) = static_cast<std::uint32_t>(texCoordIndex);
                            }

                            // has normal
                            if (parseToken(data, iterator, '/'))
                            {
                                normalIndex = parseInt32(iterator, data.end());

                                if (normalIndex < 0)
                                    normalIndex = static_cast<std::int32_t>(normals.size()) + normalIndex + 1;

                                if (normalIndex < 1 || normalIndex > static_cast<std::int32_t>(normals.size()))
                                    throw std::runtime_error("Invalid normal index");

                                std::get<2>(i) = static_cast<std::uint32_t>(normalIndex);
                            }
                        }

                        std::uint32_t index = 0;

                        auto vertexIterator = vertexMap.find(i);
                        if (vertexIterator == vertexMap.end())
                        {
                            index = static_cast<std::uint32_t>(vertices.size());
                            vertexMap[i] = index;

                            graphics::Vertex vertex;
                            if (std::get<0>(i) >= 1) vertex.position = positions[std::get<0>(i) - 1];
                            if (std::get<1>(i) >= 1) vertex.texCoords[0] = texCoords[std::get<1>(i) - 1];
                            vertex.color = Color::white();
                            if (std::get<2>(i) >= 1) vertex.normal = normals[std::get<2>(i) - 1];
                            vertices.push_back(vertex);
                            boundingBox.insertPoint(vertex.position);
                        }
                        else
                            index = vertexIterator->second;

                        vertexIndices.push_back(index);
                    }

                    if (vertexIndices.size() < 3)
                        throw std::runtime_error("Invalid face count");
                    else if (vertexIndices.size() == 3)
                        for (const auto vertexIndex : vertexIndices)
                            indices.push_back(vertexIndex);
                    else
                        for (std::uint32_t index = 0; index < vertexIndices.size() - 2; ++index)
                        {
                            indices.push_back(vertexIndices[0]);
                            indices.push_back(vertexIndices[index + 1]);
                            indices.push_back(vertexIndices[index + 2]);
                        }
                }
                else
                {
                    // skip all unknown commands
                    skipLine(iterator, data.end());
                }

                if (!objectCount) ++objectCount; // if we got at least one attribute, we have an object
            }
        }

        if (objectCount)
        {
            scene::StaticMeshData meshData(boundingBox, indices, vertices, material);
            bundle.setStaticMeshData(objectName, std::move(meshData));
        }

        return true;
    }
}
