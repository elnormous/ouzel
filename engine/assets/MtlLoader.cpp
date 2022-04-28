// Ouzel by Elviss Strazdins

#include <iterator>
#include <string>
#include "MtlLoader.hpp"
#include "AssetError.hpp"
#include "Cache.hpp"
#include "../core/Engine.hpp"

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

        void skipString(std::vector<std::byte>::const_iterator& iterator,
                        const std::vector<std::byte>::const_iterator end)
        {
            std::size_t length = 0;

            while (iterator != end && !isControlChar(*iterator) && !isWhiteSpace(*iterator))
            {
                ++length;
                ++iterator;
            }

            if (length == 0)
                throw Error{"Invalid string"};
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
                throw Error{"Invalid string"};

            return result;
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
                    throw Error{"Invalid exponent"};

                if (static_cast<char>(*iterator) == '+' ||
                    static_cast<char>(*iterator) == '-')
                    value.push_back(static_cast<char>(*iterator++));

                if (iterator == end ||
                    static_cast<char>(*iterator) < '0' ||
                    static_cast<char>(*iterator) > '9')
                    throw Error{"Invalid exponent"};

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

        void skipTextureMapOptions(std::vector<std::byte>::const_iterator& iterator,
                                   const std::vector<std::byte>::const_iterator end)
        {
            while (iterator != end)
            {
                if (static_cast<char>(*iterator) == '-')
                {
                    const auto option = parseString(iterator, end);
                    skipWhiteSpaces(iterator, end);

                    if (option == "-blende")
                        skipString(iterator, end); // on | off
                    else if (option == "-blende")
                        skipString(iterator, end); // on | off
                    else if (option == "-cc")
                        skipString(iterator, end); // on | off
                    else if (option == "-clamp")
                        skipString(iterator, end); // on | off
                    else if (option == "-mm")
                    {
                        skipString(iterator, end); // base
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // gain
                    }
                    else if (option == "-o")
                    {
                        skipString(iterator, end); // u
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // v
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // w
                    }
                    else if (option == "-s")
                    {
                        skipString(iterator, end); // u
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // v
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // w
                    }
                    else if (option == "-t")
                    {
                        skipString(iterator, end); // u
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // v
                        skipWhiteSpaces(iterator, end);
                        skipString(iterator, end); // w
                    }
                    else if (option == "-texres")
                        skipString(iterator, end); // value

                    skipWhiteSpaces(iterator, end);
                }
                else
                    break;
            }
        }
    }

    bool loadMtl(Cache& cache,
                 Bundle& bundle,
                 const std::string& name,
                 const std::vector<std::byte>& data,
                 const Asset::Options& options)
    {
        std::string materialName = name;
        std::shared_ptr<graphics::Texture> diffuseTexture;
        std::shared_ptr<graphics::Texture> ambientTexture;
        math::Color ambientColor = math::whiteColor;
        math::Color diffuseColor = math::whiteColor;
        float opacity = 1.0F;

        std::uint32_t materialCount = 0;

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

                if (const auto keyword = parseString(iterator, data.end()); keyword == "newmtl")
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

                    skipWhiteSpaces(iterator, data.end());
                    materialName = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    diffuseTexture.reset();
                    ambientTexture.reset();
                    diffuseColor = math::whiteColor;
                    opacity = 1.0F;
                }
                else if (keyword == "map_Ka") // ambient texture map
                {
                    skipWhiteSpaces(iterator, data.end());

                    skipTextureMapOptions(iterator, data.end()); // TODO: parse options

                    const auto filename = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    ambientTexture = bundle.getTexture(filename);

                    if (!ambientTexture)
                    {
                        bundle.loadAsset(Asset::Type::image, filename, filename, options);
                        ambientTexture = bundle.getTexture(filename);
                    }
                }
                else if (keyword == "map_Kd") // diffuse texture map
                {
                    skipWhiteSpaces(iterator, data.end());

                    skipTextureMapOptions(iterator, data.end()); // TODO: parse options

                    const auto filename = parseString(iterator, data.end());

                    skipLine(iterator, data.end());

                    diffuseTexture = bundle.getTexture(filename);

                    if (!diffuseTexture)
                    {
                        bundle.loadAsset(Asset::Type::image, filename, filename, options);
                        diffuseTexture = bundle.getTexture(filename);
                    }
                }
                else if (keyword == "Ka") // ambient color
                {
                    // TODO: handle spectral and xyz

                    skipWhiteSpaces(iterator, data.end());
                    const auto red = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto green = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto blue = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    ambientColor = math::Color{red, green, blue};
                }
                else if (keyword == "Kd") // diffuse color
                {
                    // TODO: handle spectral and xyz

                    skipWhiteSpaces(iterator, data.end());
                    const auto red = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto green = parseFloat(iterator, data.end());
                    skipWhiteSpaces(iterator, data.end());
                    const auto blue = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    diffuseColor = math::Color{red, green, blue};
                }
                else if (keyword == "Ks") // specular color
                    skipLine(iterator, data.end());
                else if (keyword == "Ke") // emissive color
                    skipLine(iterator, data.end());
                else if (keyword == "d") // dissolve
                {
                    skipWhiteSpaces(iterator, data.end());
                    opacity = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());
                }
                else if (keyword == "Tr") // transparency
                {
                    skipWhiteSpaces(iterator, data.end());
                    const auto transparency = parseFloat(iterator, data.end());

                    skipLine(iterator, data.end());

                    // d = 1 - Tr
                    opacity = 1.0F - transparency;
                }
                else if (keyword == "Tf") // transmission filter
                    skipLine(iterator, data.end());
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
            material->ambientColor = ambientColor;
            material->diffuseColor = diffuseColor;
            material->opacity = opacity;
            material->cullMode = graphics::CullMode::back;

            bundle.setMaterial(materialName, std::move(material));
        }

        return true;
    }
}
