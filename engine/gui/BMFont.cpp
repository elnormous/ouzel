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
        constexpr auto isWhiteSpace(const std::byte c) noexcept
        {
            return static_cast<char>(c) == ' ' ||
                static_cast<char>(c) == '\t';
        }

        constexpr auto isNewline(const std::byte c) noexcept
        {
            return static_cast<char>(c) == '\r' ||
                static_cast<char>(c) == '\n';
        }

        constexpr auto isControlChar(const std::byte c) noexcept
        {
            return static_cast<std::uint8_t>(c) <= 0x1F;
        }

        void skipWhiteSpaces(const std::vector<std::byte>& str,
                             std::vector<std::byte>::const_iterator& iterator) noexcept
        {
            while (iterator != str.end())
                if (isWhiteSpace(*iterator))
                    ++iterator;
                else
                    break;
        }

        void skipLine(const std::vector<std::byte>& str,
                      std::vector<std::byte>::const_iterator& iterator) noexcept
        {
            while (iterator != str.end())
                if (isNewline(*iterator++))
                    break;
        }

        std::string parseString(const std::vector<std::byte>& str,
                                std::vector<std::byte>::const_iterator& iterator)
        {
            if (iterator == str.end())
                throw std::runtime_error{"Invalid string"};

            std::string result;

            if (static_cast<char>(*iterator) == '"')
            {
                ++iterator;

                for (;;)
                {
                    if (static_cast<char>(*iterator) == '"' &&
                        (iterator + 1 == str.end() ||
                         isWhiteSpace(*(iterator + 1)) ||
                         isNewline(*(iterator + 1))))
                    {
                        ++iterator;
                        break;
                    }
                    if (iterator == str.end())
                        throw std::runtime_error{"Unterminated string"};

                    result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }
            }
            else
            {
                while (iterator != str.end() &&
                       !isControlChar(*iterator) &&
                       !isWhiteSpace(*iterator) &&
                       static_cast<char>(*iterator) != '=')
                {
                    result.push_back(static_cast<char>(*iterator));

                    ++iterator;
                }

                if (result.empty())
                    throw std::runtime_error{"Invalid string"};
            }

            return result;
        }

        void skipString(const std::vector<std::byte>& str,
                        std::vector<std::byte>::const_iterator& iterator)
        {
            if (iterator == str.end())
                throw std::runtime_error{"Invalid string"};

            if (static_cast<char>(*iterator) == '"')
            {
                ++iterator;

                for (;;)
                {
                    if (static_cast<char>(*iterator) == '"' &&
                        (iterator + 1 == str.end() ||
                         isWhiteSpace(*(iterator + 1)) ||
                         isNewline(*(iterator + 1))))
                    {
                        ++iterator;
                        break;
                    }
                    if (iterator == str.end())
                        throw std::runtime_error{"Unterminated string"};

                    ++iterator;
                }
            }
            else
            {
                bool empty = true;

                while (iterator != str.end() &&
                       !isControlChar(*iterator) &&
                       !isWhiteSpace(*iterator) &&
                       static_cast<char>(*iterator) != '=')
                {
                    ++iterator;
                    empty = false;
                }

                if (empty)
                    throw std::runtime_error{"Invalid string"};
            }
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
                throw std::runtime_error{"Invalid integer"};

            return result;
        }

        void expectToken(const std::vector<std::byte>& str,
                         std::vector<std::byte>::const_iterator& iterator,
                         char token)
        {
            if (iterator == str.end() ||
                static_cast<char>(*iterator) != token)
                throw std::runtime_error{"Unexpected token"};

            ++iterator;
        }
    }

    BMFont::BMFont(const std::vector<std::byte>& data)
    {
        auto iterator = data.cbegin();

        std::string keyword;
        std::string key;

        while (iterator != data.end())
        {
            if (isNewline(*iterator))
            {
                // skip empty lines
                ++iterator;
            }
            else
            {
                skipWhiteSpaces(data, iterator);
                keyword = parseString(data, iterator);

                if (keyword == "page")
                {
                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhiteSpaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');
                        std::string value = parseString(data, iterator);

                        if (key == "file")
                            fontTexture = engine->getCache().getTexture(value);
                    }
                }
                else if (keyword == "common")
                {
                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhiteSpaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "lineHeight")
                            lineHeight = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "base")
                            base = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "scaleW")
                            width = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "scaleH")
                            height = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "pages")
                            pages = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "outline")
                            outline = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else
                            skipString(data, iterator);
                    }
                }
                else if (keyword == "char")
                {
                    char32_t charId = 0;
                    CharDescriptor c;

                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhiteSpaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "id")
                            charId = static_cast<std::uint32_t>(std::stoul(parseInt(data, iterator)));
                        else if (key == "x")
                            c.x = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "y")
                            c.y = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "width")
                            c.width = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "height")
                            c.height = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "xoffset")
                            c.xOffset = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "yoffset")
                            c.yOffset = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "xadvance")
                            c.xAdvance = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else if (key == "page")
                            c.page = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else
                            skipString(data, iterator);
                    }

                    chars.insert(std::unordered_map<std::int32_t, CharDescriptor>::value_type(charId, c));
                }
                else if (keyword == "kernings")
                {
                    while (iterator != data.end())
                    {
                        if (isNewline(*iterator)) break;

                        skipWhiteSpaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "count")
                            kernCount = static_cast<std::uint16_t>(std::stoi(parseInt(data, iterator)));
                        else
                            skipString(data, iterator);
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

                        skipWhiteSpaces(data, iterator);
                        key = parseString(data, iterator);

                        expectToken(data, iterator, '=');

                        if (key == "first")
                            first = static_cast<char32_t>(std::stoul(parseInt(data, iterator)));
                        else if (key == "second")
                            second = static_cast<char32_t>(std::stoul(parseInt(data, iterator)));
                        else if (key == "amount")
                            amount = static_cast<std::int16_t>(std::stoi(parseInt(data, iterator)));
                        else
                            skipString(data, iterator);

                        kern[std::pair{first, second}] = amount;
                    }
                }
                else
                    skipLine(data, iterator);
            }
        }
    }

    Font::RenderData BMFont::getRenderData(const std::string_view& text,
                                           math::Color color,
                                           float fontSize,
                                           const math::Vector<float, 2>& anchor) const
    {
        math::Vector<float, 2> position{};

        const auto utf32Text = utf8::toUtf32(text);

        std::vector<std::uint16_t> indices;
        std::vector<graphics::Vertex> vertices;
        indices.reserve(utf32Text.size() * 6);
        vertices.reserve(utf32Text.size() * 4);

        std::size_t firstChar = 0;

        for (auto i = utf32Text.begin(); i != utf32Text.end(); ++i)
        {
            if (const auto iterator = chars.find(*i); iterator != chars.end())
            {
                const auto& f = iterator->second;

                const auto startIndex = static_cast<std::uint16_t>(vertices.size());
                indices.push_back(startIndex + 0);
                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 2);

                indices.push_back(startIndex + 1);
                indices.push_back(startIndex + 3);
                indices.push_back(startIndex + 2);

                const math::Vector<float, 2> leftTop{
                    f.x / static_cast<float>(width),
                    f.y / static_cast<float>(height)
                };

                const math::Vector<float, 2> rightBottom{
                    (f.x + f.width) / static_cast<float>(width),
                    (f.y + f.height) / static_cast<float>(height)
                };

                const std::array<math::Vector<float, 2>, 4> textCoords{
                    math::Vector<float, 2>{leftTop.v[0], rightBottom.v[1]},
                    math::Vector<float, 2>{rightBottom.v[0], rightBottom.v[1]},
                    math::Vector<float, 2>{leftTop.v[0], leftTop.v[1]},
                    math::Vector<float, 2>{rightBottom.v[0], leftTop.v[1]}
                };

                vertices.emplace_back(math::Vector<float, 3>{position.v[0] + f.xOffset, -position.v[1] - f.yOffset - f.height, 0.0F},
                                      color, textCoords[0], math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(math::Vector<float, 3>{position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset - f.height, 0.0F},
                                      color, textCoords[1], math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(math::Vector<float, 3>{position.v[0] + f.xOffset, -position.v[1] - f.yOffset, 0.0F},
                                      color, textCoords[2], math::Vector<float, 3>{0.0F, 0.0F, -1.0F});
                vertices.emplace_back(math::Vector<float, 3>{position.v[0] + f.xOffset + f.width, -position.v[1] - f.yOffset, 0.0F},
                                      color, textCoords[3], math::Vector<float, 3>{0.0F, 0.0F, -1.0F});

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

        const auto textHeight = position.v[1];

        for (auto& vertex : vertices)
        {
            vertex.position.v[1] += textHeight * (1.0F - anchor.v[1]);

            vertex.position.v[0] *= fontSize;
            vertex.position.v[1] *= fontSize;
        }

        return std::make_tuple(std::move(indices), std::move(vertices), fontTexture);
    }

    std::int16_t BMFont::getKerningPair(char32_t first, char32_t second) const
    {
        if (const auto i = kern.find(std::pair{first, second}); i != kern.end())
            return i->second;

        return 0;
    }
}
