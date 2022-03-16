// Ouzel by Elviss Strazdins

#include <cstdlib>
#include <map>
#include <stdexcept>
#include <tuple>
#include "ObjLoader.hpp"
#include "../graphics/Material.hpp"

namespace ouzel::assets
{
    namespace
    {
        [[nodiscard]] constexpr auto isWhiteSpace(const std::byte c) noexcept
        {
            return static_cast<char>(c) == ' ' ||
                static_cast<char>(c) == '\t';
        }

        [[nodiscard]] constexpr auto isNewline(const std::byte c) noexcept
        {
            return static_cast<char>(c) == '\r' ||
                static_cast<char>(c) == '\n';
        }

        [[nodiscard]] constexpr auto isControlChar(const std::byte c) noexcept
        {
            return static_cast<std::uint8_t>(c) <= 0x1F;
        }

        void skipWhiteSpaces(std::vector<std::byte>::const_iterator& iterator,
                             const std::vector<std::byte>::const_iterator end) noexcept
        {
            while (iterator != end)
                if (isWhiteSpace(*iterator))
                    ++iterator;
                else
                    break;
        }

        void skipLine(std::vector<std::byte>::const_iterator& iterator,
                      const std::vector<std::byte>::const_iterator end) noexcept
        {
            while (iterator != end)
                if (isNewline(*iterator++))
                    break;
        }

        [[nodiscard]] std::string parseString(std::vector<std::byte>::const_iterator& iterator,
                                              const std::vector<std::byte>::const_iterator end)
        {
            std::string result;

            while (iterator != end && !isControlChar(*iterator) && !isWhiteSpace(*iterator))
            {
                result.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            if (result.empty())
                throw std::runtime_error{"Invalid string"};

            return result;
        }

        [[nodiscard]] std::int32_t parseInt32(std::vector<std::byte>::const_iterator& iterator,
                                              const std::vector<std::byte>::const_iterator end)
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

        [[nodiscard]] float parseFloat(std::vector<std::byte>::const_iterator& iterator,
                                       const std::vector<std::byte>::const_iterator end)
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
                    throw std::runtime_error{"Invalid exponent"};

                if (static_cast<char>(*iterator) == '+' ||
                    static_cast<char>(*iterator) == '-')
                    value.push_back(static_cast<char>(*iterator++));

                if (iterator == end ||
                    static_cast<char>(*iterator) < '0' ||
                    static_cast<char>(*iterator) > '9')
                    throw std::runtime_error{"Invalid exponent"};

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

        [[nodiscard]] bool parseToken(const std::vector<std::byte>& str,
                                      std::vector<std::byte>::const_iterator& iterator,
                                      const char token)
        {
            if (iterator == str.end() || static_cast<char>(*iterator) != token) return false;

            ++iterator;

            return true;
        }
    }

    bool loadObj(Cache&,
                 Bundle& bundle,
                 const std::string& name,
                 const std::vector<std::byte>& data,
                 const Asset::Options& options)
    {
        std::string objectName = name;
        const graphics::Material* material = nullptr;
        std::vector<math::Vector<float, 3>> positions;
        std::vector<math::Vector<float, 2>> texCoords;
        std::vector<math::Vector<float, 3>> normals;
        std::vector<graphics::Vertex> vertices;
        std::map<std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::uint32_t> vertexMap;
        std::vector<std::uint32_t> indices;
        math::Box<float, 3> boundingBox;

        std::uint32_t objectCount = 0;

        for (auto iterator = data.cbegin(); iterator != data.end();)
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
                skipWhiteSpaces(iterator, data.end());

                if (const auto keyword = parseString(iterator, data.end()); keyword == "mtllib")
                {
                    skipWhiteSpaces(iterator, data.end());
                    const auto filename = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    //if (!cache.getMaterial(filename))
                    // TODO: don't load material lib every time
                    bundle.loadAsset(Asset::Type::material, filename, filename, options);
                }
                else if (keyword == "usemtl")
                {
                    skipWhiteSpaces(iterator, data.end());
                    const auto materialName = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    material = bundle.getMaterial(materialName);
                }
                else if (keyword == "o")
                {
                    if (objectCount)
                    {
                        scene::StaticMeshData meshData(boundingBox, indices, vertices, material);
                        bundle.setStaticMeshData(objectName, std::move(meshData));
                    }

                    skipWhiteSpaces(iterator, data.end());
                    objectName = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    material = nullptr;
                    vertices.clear();
                    indices.clear();
                    vertexMap.clear();
                    reset(boundingBox);
                    ++objectCount;
                }
                else if (keyword == "v")
                {
                    skipWhiteSpaces(iterator, data.end());
                    const auto x = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto y = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto z = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    positions.push_back(math::Vector<float, 3>{x, y, z});
                }
                else if (keyword == "vt")
                {
                    skipWhiteSpaces(iterator, data.end());
                    const auto u = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto v = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    texCoords.push_back(math::Vector<float, 2>{u, v});
                }
                else if (keyword == "vn")
                {
                    skipWhiteSpaces(iterator, data.end());
                    const auto x = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto y = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto z = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    normals.push_back(math::Vector<float, 3>{x, y, z});
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

                        skipWhiteSpaces(iterator, data.end());
                        positionIndex = parseInt32(iterator, data.end());

                        if (positionIndex < 0)
                            positionIndex = static_cast<std::int32_t>(positions.size()) + positionIndex + 1;

                        if (positionIndex < 1 || positionIndex > static_cast<std::int32_t>(positions.size()))
                            throw std::runtime_error{"Invalid position index"};

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
                                    throw std::runtime_error{"Invalid texture coordinate index"};

                                std::get<1>(i) = static_cast<std::uint32_t>(texCoordIndex);
                            }

                            // has normal
                            if (parseToken(data, iterator, '/'))
                            {
                                normalIndex = parseInt32(iterator, data.end());

                                if (normalIndex < 0)
                                    normalIndex = static_cast<std::int32_t>(normals.size()) + normalIndex + 1;

                                if (normalIndex < 1 || normalIndex > static_cast<std::int32_t>(normals.size()))
                                    throw std::runtime_error{"Invalid normal index"};

                                std::get<2>(i) = static_cast<std::uint32_t>(normalIndex);
                            }
                        }

                        std::uint32_t index = 0;

                        if (const auto vertexIterator = vertexMap.find(i); vertexIterator == vertexMap.end())
                        {
                            index = static_cast<std::uint32_t>(vertices.size());
                            vertexMap[i] = index;

                            graphics::Vertex vertex;
                            if (std::get<0>(i) >= 1) vertex.position = positions[std::get<0>(i) - 1];
                            if (std::get<1>(i) >= 1) vertex.texCoords[0] = texCoords[std::get<1>(i) - 1];
                            vertex.color = math::whiteColor;
                            if (std::get<2>(i) >= 1) vertex.normal = normals[std::get<2>(i) - 1];
                            vertices.push_back(vertex);
                            insertPoint(boundingBox, vertex.position);
                        }
                        else
                            index = vertexIterator->second;

                        vertexIndices.push_back(index);
                    }

                    if (vertexIndices.size() < 3)
                        throw std::runtime_error{"Invalid face count"};
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
