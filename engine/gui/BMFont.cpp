// Ouzel by Elviss Strazdins

#include <string>
#include <iterator>
#include <stdexcept>
#include "BMFont.hpp"
#include "../core/Engine.hpp"
#include "../assets/Cache.hpp"
#include "../utils/Utf8.hpp"

namespace ouzel::gui
{
    namespace
    {
        constexpr auto isWhitespace(std::byte c) noexcept
        {
            return static_cast<char>(c) == ' ' ||
                static_cast<char>(c) == '\t';
        }

        constexpr auto isNewline(std::byte c) noexcept
        {
            return static_cast<char>(c) == '\r' ||
                static_cast<char>(c) == '\n';
        }

        constexpr auto isControlChar(std::byte c) noexcept
        {
            return static_cast<std::uint8_t>(c) <= 0x1F;
        }

        void skipWhitespaces(const std::vector<std::byte>& str,
                             std::vector<std::byte>::const_iterator& iterator) noexcept
        {
            while (iterator != str.end())
                if (isWhitespace(*iterator))
                    ++iterator;
                else
                    break;
        }

        void skipLine(const std::vector<std::byte>& str,
                      std::vector<std::byte>::const_iterator& iterator) noexcept
        {
            while (iterator != str.end())
            {
                if (isNewline(*iterator))
                {
                    ++iterator;
                    break;
                }

                ++iterator;
            }
        }

        std::string parseString(const std::vector<std::byte>& str,
                                std::vector<std::byte>::const_iterator& iterator)
        {
            if (iterator == str.end())
                throw std::runtime_error("Invalid string");

            std::string result;

            if (static_cast<char>(*iterator) == '"')
            {
                ++iterator;

                for (;;)
                {
                    if (static_cast<char>(*iterator) == '"' &&
                        (iterator + 1 == str.end() ||
                         isWhitespace(*(iterator + 1)) ||
                         isNewline(*(iterator + 1))))
                    {
                        ++iterator;
                        break;
                    }
                    if (iterator == str.end())
                        throw std::runtime_error("Unterminated string");

                    result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }
            }
            else
            {
                while (iterator != str.end() &&
                       !isControlChar(*iterator) &&
                       !isWhitespace(*iterator) &&
                       static_cast<char>(*iterator) != '=')
                {
                    result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }

                if (result.empty())
                    throw std::runtime_error("Invalid string");
            }

            return result;
        }

        std::string parseInt(const std::vector<std::byte>& str,
                             std::vector<std::byte>::const_iterator& iterator)
        {
            std::string result;
            std::uint32_t length = 1;

            if (iterator != str.end() &&
                static_cast<char>(*iterator) == '-')
            {
                result.push_back(static_cast<char>(*iterator));
                ++length;
                ++iterator;
            }

            while (iterator != str.end() &&
                   static_cast<char>(*iterator) >= '0' &&
                   static_cast<char>(*iterator) <= '9')
            {
                result.push_back(static_cast<char>(*iterator));

                ++iterator;
            }

            if (result.length() < length)
                throw std::runtime_error("Invalid integer");

            return result;
        }

        void expectToken(const std::vector<std::byte>& str,
                         std::vector<std::byte>::const_iterator& iterator,
                         char token)
        {
            if (iterator == str.end() ||
                static_cast<char>(*iterator) != token)
                throw std::runtime_error("Unexpected token");

            ++iterator;
        }
    }

    BMFont::BMFont(const std::vector<std::byte>& data)
    {
        auto iterator = data.cbegin();

        std::string keyword;
        std::string key;
        std::string value;

        while (iterator != data.end())
        {
            if (isNewline(*iterator))
            {
                // skip empty lines
                ++iterator;
            }
            else
            {
                skipWhitespaces(data, iterator);
                keyword = parseString(data, iterator);

                if (keyword == "page")
                {
                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhitespaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');
                        value = parseString(data, iterator);

                        if (key == "file")
                            fontTexture = engine->getCache().getTexture(value);
                    }
                }
                else if (keyword == "common")
                {
                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhitespaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "lineHeight")
                        {
                            value = parseInt(data, iterator);
                            lineHeight = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else if (key == "base")
                        {
                            value = parseInt(data, iterator);
                            base = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else if (key == "scaleW")
                        {
                            value = parseInt(data, iterator);
                            width = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else if (key == "scaleH")
                        {
                            value = parseInt(data, iterator);
                            height = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else if (key == "pages")
                        {
                            value = parseInt(data, iterator);
                            pages = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else if (key == "outline")
                        {
                            value = parseInt(data, iterator);
                            outline = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else
                            value = parseString(data, iterator);
                    }
                }
                else if (keyword == "char")
                {
                    char32_t charId = 0;
                    CharDescriptor c;

                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhitespaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "id")
                        {
                            value = parseInt(data, iterator);
                            charId = static_cast<std::uint32_t>(std::stoul(value));
                        }
                        else if (key == "x")
                        {
                            value = parseInt(data, iterator);
                            c.x = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "y")
                        {
                            value = parseInt(data, iterator);
                            c.y = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "width")
                        {
                            value = parseInt(data, iterator);
                            c.width = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "height")
                        {
                            value = parseInt(data, iterator);
                            c.height = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "xoffset")
                        {
                            value = parseInt(data, iterator);
                            c.xOffset = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "yoffset")
                        {
                            value = parseInt(data, iterator);
                            c.yOffset = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "xadvance")
                        {
                            value = parseInt(data, iterator);
                            c.xAdvance = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else if (key == "page")
                        {
                            value = parseInt(data, iterator);
                            c.page = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else
                            value = parseString(data, iterator);
                    }

                    chars.insert(std::unordered_map<std::int32_t, CharDescriptor>::value_type(charId, c));
                }
                else if (keyword == "kernings")
                {
                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhitespaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "count")
                        {
                            value = parseInt(data, iterator);
                            kernCount = static_cast<std::uint16_t>(std::stoi(value));
                        }
                        else
                            value = parseString(data, iterator);
                    }
                }
                else if (keyword == "kerning")
                {
                    std::int16_t amount = 0;
                    char32_t first = 0;
                    char32_t second = 0;

                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhitespaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "first")
                        {
                            value = parseInt(data, iterator);
                            first = static_cast<char32_t>(std::stoul(value));
                        }
                        else if (key == "second")
                        {
                            value = parseInt(data, iterator);
                            second = static_cast<char32_t>(std::stoul(value));
                        }
                        else if (key == "amount")
                        {
                            value = parseInt(data, iterator);
                            amount = static_cast<std::int16_t>(std::stoi(value));
                        }
                        else
                            value = parseString(data, iterator);

                        kern[std::pair(first, second)] = amount;
                    }
                }
                else
                    skipLine(data, iterator);
            }
        }
    }

    Font::RenderData BMFont::getRenderData(const std::string& text,
                                           Color color,
                                           float fontSize,
                                           const Vector2F& anchor) const
    {
        Vector2F position;

        const std::u32string utf32Text = utf8::toUtf32(text);

        std::vector<std::uint16_t> indices;
        std::vector<graphics::Vertex> vertices;
        indices.reserve(utf32Text.size() * 6);
        vertices.reserve(utf32Text.size() * 4);

        std::size_t firstChar = 0;

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            const auto iterator = chars.find(*i);

            if (iterator != chars.end())
            {
                const auto& f = iterator->second;

                const auto startIndex = static_cast<std::uint16_t>(vertices.size());
                indices.push_back(startIndex + 0);
                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 2);

                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 3);
                indices.push_back(startIndex + 2);

                const Vector2F leftTop{
                    f.x / static_cast<float>(width),
                    f.y / static_cast<float>(height)
                };

                const Vector2F rightBottom{
                    (f.x + f.width) / static_cast<float>(width),
                    (f.y + f.height) / static_cast<float>(height)
                };

                const std::array<Vector2F, 4> textCoords{
                    Vector2F{leftTop.v[0], rightBottom.v[1]},
                    Vector2F{rightBottom.v[0], rightBottom.v[1]},
                    Vector2F{leftTop.v[0], leftTop.v[1]},
                    Vector2F{rightBottom.v[0], leftTop.v[1]}
                };

                vertices.emplace_back(Vector3F{position.v[0] + f.xOffset, -position.v[1] - f.yOffset - f.height, 0.0F},
                                      color, textCoords[0], Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset - f.height, 0.0F},
                                      color, textCoords[1], Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{position.v[0] + f.xOffset, -position.v[1] - f.yOffset, 0.0F},
                                      color, textCoords[2], Vector3F{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(Vector3F{position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset, 0.0F},
                                      color, textCoords[3], Vector3F{0.0F, 0.0F, -1.0F});

                if ((i + 1) != utf32Text.end())
                    position.v[0] += static_cast<float>(getKerningPair(*i, *(i + 1)));

                position.v[0] += f.xAdvance;
            }

            if (*i == '\n' || // line feed
                (i + 1) == utf32Text.end()) // end of string
            {
                const float lineWidth = position.v[0];
                position.v[0] = 0.0F;
                position.v[1] += lineHeight;

                for (std::size_t c = firstChar; c < vertices.size(); ++c)
                    vertices[c].position.v[0] -= lineWidth * anchor.v[0];

                firstChar = vertices.size();
            }
        }

        const float textHeight = position.v[1];

        for (graphics::Vertex& vertex : vertices)
        {
            vertex.position.v[1] += textHeight * (1.0F - anchor.v[1]);

            vertex.position.v[0] *= fontSize;
            vertex.position.v[1] *= fontSize;
        }

        return std::make_tuple(std::move(indices), std::move(vertices), fontTexture);
    }

    std::int16_t BMFont::getKerningPair(char32_t first, char32_t second) const
    {
        const auto i = kern.find(std::pair(first, second));

        if (i != kern.end())
            return i->second;

        return 0;
    }

    float BMFont::getStringWidth(const std::string& text) const
    {
        float total = 0.0F;

        const std::u32string utf32Text = utf8::toUtf32(text);

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            const auto iter = chars.find(*i);

            if (iter != chars.end())
            {
                if ((i + 1) != utf32Text.end())
                    total += static_cast<float>(getKerningPair(*i, *(i + 1)));

                const auto& f = iter->second;
                total += f.xAdvance;
            }
        }

        return total;
    }
}
