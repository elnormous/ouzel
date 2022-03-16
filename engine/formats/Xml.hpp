// Ouzel by Elviss Strazdins

#ifndef OUZEL_FORMATS_XML_HPP
#define OUZEL_FORMATS_XML_HPP

#include <array>
#include <cctype>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "../utils/Utf8.hpp"

namespace ouzel::xml
{
    class ParseError final: public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    class RangeError final: public std::range_error
    {
    public:
        using std::range_error::range_error;
    };

    using Attributes = std::map<std::string, std::string, std::less<>>;

    class Node final
    {
    public:
        enum class Type
        {
            comment,
            characterData,
            processingInstruction,
            documentTypeDefinition,
            element,
            attributeList,
            entity,
            notation,
            tag,
            text
        };

        enum class ExternalIdType
        {
            none,
            system,
            pub
        };

        Node() = default;
        Node(const Type initType): type{initType} {}
        Node(std::string&& val) noexcept: type{Type::text}, value{std::move(val)} {}

        template <class Source>
        Node(const Source& val): type{Type::text}, value{std::string{val}} {}

        Node& operator=(const Type newType) noexcept
        {
            type = newType;
            return *this;
        }

        Node& operator=(std::string&& val) noexcept
        {
            type = Type::text;
            value = std::move(val);
            return *this;
        }

        template <class Source>
        Node& operator=(const Source& val)
        {
            type = Type::text;
            value = std::string{val};
            return *this;
        }

        Type getType() const noexcept { return type; }
        void setType(const Type newType) noexcept { type = newType; }

        auto begin() noexcept
        {
            return children.begin();
        }

        auto end() noexcept
        {
            return children.end();
        }

        auto begin() const noexcept
        {
            return children.begin();
        }

        auto end() const noexcept
        {
            return children.end();
        }

        const auto& operator[](const std::string_view attribute) const
        {
            if (const auto iterator = attributes.find(attribute); iterator != attributes.end())
                return iterator->second;
            else
                throw RangeError{"Invalid attribute"};
        }

        auto& operator[](const std::string_view attribute) noexcept
        {
            if (const auto iterator = attributes.find(attribute); iterator != attributes.end())
                return iterator->second;
            else
            {
                const auto [newIterator, success] = attributes.insert(std::make_pair(attribute, std::string{}));
                (void)success;
                return newIterator->second;
            }
        }

        const auto& getChildren() const noexcept { return children; }
        void pushBack(const Node& node) { children.push_back(node); }

        const auto& getName() const noexcept { return name; }
        void setName(const std::string_view newName) { name = newName; }

        const auto& getExternalIdType() const noexcept { return externalIdType; }
        void setExternalIdType(const ExternalIdType newExternalIdType) { externalIdType = newExternalIdType; }

        const auto& getValue() const noexcept { return value; }
        void setValue(const std::string_view newValue) { value = newValue; }

        const auto& getAttributes() const noexcept { return attributes; }
        void setAttributes(const Attributes& newAttributes) { attributes = newAttributes; }

    private:
        Type type = Type::tag;
        std::string name;
        ExternalIdType externalIdType;
        std::string value;
        Attributes attributes;
        std::vector<Node> children;
    };

    class Data final
    {
    public:
        Data() = default;

        auto begin() noexcept
        {
            return children.begin();
        }

        auto end() noexcept
        {
            return children.end();
        }

        auto begin() const noexcept
        {
            return children.begin();
        }

        auto end() const noexcept
        {
            return children.end();
        }

        const auto& getChildren() const noexcept { return children; }
        void pushBack(const Node& node) { children.push_back(node); }

    private:
        std::vector<Node> children;
    };

    inline namespace detail
    {
        constexpr std::array<std::uint8_t, 3> utf8ByteOrderMark = {0xEF, 0xBB, 0xBF};
    }

    template <class Iterator>
    Data parse(const Iterator begin, const Iterator end,
               const bool preserveWhitespaces = false,
               const bool preserveComments = false,
               const bool preserveProcessingInstructions = false)
    {
        class Parser final
        {
        public:
            static std::u32string toUtf32(const Iterator begin, const Iterator end)
            {
                std::u32string result;

                for (auto i = begin; i != end; ++i)
                {
                    char32_t cp = static_cast<char32_t>(*i) & 0xFF;

                    if (cp <= 0x7F) // length = 1
                    {
                        // do nothing
                    }
                    else if ((cp >> 5) == 0x6) // length = 2
                    {
                        if (++i == end)
                            throw ParseError{"Invalid UTF-8 string"};
                        cp = ((cp << 6) & 0x7FF) + (static_cast<char32_t>(*i) & 0x3F);
                    }
                    else if ((cp >> 4) == 0xE) // length = 3
                    {
                        if (++i == end)
                            throw ParseError{"Invalid UTF-8 string"};
                        cp = ((cp << 12) & 0xFFFF) + (((static_cast<char32_t>(*i) & 0xFF) << 6) & 0x0FFF);
                        if (++i == end)
                            throw ParseError{"Invalid UTF-8 string"};
                        cp += static_cast<char32_t>(*i) & 0x3F;
                    }
                    else if ((cp >> 3) == 0x1E) // length = 4
                    {
                        if (++i == end)
                            throw ParseError{"Invalid UTF-8 string"};
                        cp = ((cp << 18) & 0x1FFFFF) + (((static_cast<char32_t>(*i) & 0xFF) << 12) & 0x3FFFF);
                        if (++i == end)
                            throw ParseError{"Invalid UTF-8 string"};
                        cp += ((static_cast<char32_t>(*i) & 0xFF) << 6) & 0x0FFF;
                        if (++i == end)
                            throw ParseError{"Invalid UTF-8 string"};
                        cp += static_cast<char32_t>(*i) & 0x3F;
                    }

                    result.push_back(cp);
                }

                return result;
            }

            static std::string fromUtf32(const char32_t c)
            {
                std::string result;

                if (c <= 0x7F)
                    result.push_back(static_cast<char>(c));
                else if (c <= 0x7FF)
                {
                    result.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
                else if (c <= 0xFFFF)
                {
                    result.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
                else
                {
                    result.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                    result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }

                return result;
            }

            static std::string fromUtf32(const std::u32string::const_iterator begin,
                                         const std::u32string::const_iterator end)
            {
                std::string result;

                for (auto i = begin; i != end; ++i)
                {
                    if (*i <= 0x7F)
                        result.push_back(static_cast<char>(*i));
                    else if (*i <= 0x7FF)
                    {
                        result.push_back(static_cast<char>(0xC0 | ((*i >> 6) & 0x1F)));
                        result.push_back(static_cast<char>(0x80 | (*i & 0x3F)));
                    }
                    else if (*i <= 0xFFFF)
                    {
                        result.push_back(static_cast<char>(0xE0 | ((*i >> 12) & 0x0F)));
                        result.push_back(static_cast<char>(0x80 | ((*i >> 6) & 0x3F)));
                        result.push_back(static_cast<char>(0x80 | (*i & 0x3F)));
                    }
                    else
                    {
                        result.push_back(static_cast<char>(0xF0 | ((*i >> 18) & 0x07)));
                        result.push_back(static_cast<char>(0x80 | ((*i >> 12) & 0x3F)));
                        result.push_back(static_cast<char>(0x80 | ((*i >> 6) & 0x3F)));
                        result.push_back(static_cast<char>(0x80 | (*i & 0x3F)));
                    }
                }

                return result;
            }

            static std::string fromUtf32(const std::u32string& text)
            {
                return fromUtf32(std::begin(text), std::end(text));
            }

            static Data parse(const Iterator begin, const Iterator end,
                              const bool preserveWhitespaces,
                              const bool preserveComments,
                              const bool preserveProcessingInstructions)
            {
                bool byteOrderMark = hasByteOrderMark(begin, end);

                const auto str = toUtf32(byteOrderMark ? begin + 3 : begin, end);
                auto iterator = str.begin();
                bool rootTagFound = false;
                bool prologAllowed = true;

                Data result;

                for (;;)
                {
                    if (!preserveWhitespaces) skipWhitespaces(iterator, str.end());

                    if (iterator == str.end()) break;

                    const auto node = parse(iterator, str.end(),
                                            preserveWhitespaces,
                                            preserveComments,
                                            preserveProcessingInstructions,
                                            prologAllowed);

                    if ((preserveComments || node.getType() != Node::Type::comment) &&
                        (preserveProcessingInstructions || node.getType() != Node::Type::processingInstruction))
                    {
                        result.pushBack(node);

                        if (node.getType() == Node::Type::tag)
                        {
                            if (rootTagFound)
                                throw ParseError{"Multiple root tags found"};
                            else
                                rootTagFound = true;
                        }
                    }

                    prologAllowed = false;
                }

                if (!rootTagFound)
                    throw ParseError{"No root tag found"};

                return result;
            }

        private:
            static bool hasByteOrderMark(const Iterator begin, const Iterator end) noexcept
            {
                for (const auto b : utf8ByteOrderMark)
                    if (begin == end || static_cast<std::uint8_t>(*begin) != b)
                        return false;
                    else
                        ++begin;
                return true;
            }

            static constexpr bool isWhitespace(const char32_t c) noexcept
            {
                return c == ' ' || c == '\t' || c == '\r' || c == '\n';
            }

            static constexpr bool isNameStartChar(const char32_t c) noexcept
            {
                return (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    c == ':' || c == '_' ||
                    (c >= 0xC0 && c <= 0xD6) ||
                    (c >= 0xD8 && c <= 0xF6) ||
                    (c >= 0xF8 && c <= 0x2FF) ||
                    (c >= 0x370 && c <= 0x37D) ||
                    (c >= 0x37F && c <= 0x1FFF) ||
                    (c >= 0x200C && c <= 0x200D) ||
                    (c >= 0x2070 && c <= 0x218F) ||
                    (c >= 0x2C00 && c <= 0x2FEF) ||
                    (c >= 0x3001 && c <= 0xD7FF) ||
                    (c >= 0xF900 && c <= 0xFDCF) ||
                    (c >= 0xFDF0 && c <= 0xFFFD) ||
                    (c >= 0x10000 && c <= 0xEFFFF);
            }

            static constexpr bool isNameChar(const char32_t c) noexcept
            {
                return isNameStartChar(c) ||
                    c == '-' || c == '.' ||
                    (c >= '0' && c <= '9') ||
                    c == 0xB7 ||
                    (c >= 0x0300 && c <= 0x036F) ||
                    (c >= 0x203F && c <= 0x2040);
            }

            static void skipWhitespaces(std::u32string::const_iterator& iterator,
                                        const std::u32string::const_iterator end)
            {
                while (iterator != end && isWhitespace(*iterator))
                    ++iterator;
            }

            static void expect(std::u32string::const_iterator& iterator,
                               const std::u32string::const_iterator end,
                               const char32_t c)
            {
                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                if (*iterator != c)
                    throw ParseError{"Unexpected character"};

                ++iterator;
            }

            static std::string parseName(std::u32string::const_iterator& iterator,
                                         const std::u32string::const_iterator end)
            {
                std::string result;

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                if (!isNameStartChar(*iterator))
                    throw ParseError{"Invalid name start"};

                while (isNameChar(*iterator))
                {
                    result += fromUtf32(*iterator);

                    if (++iterator == end)
                        throw ParseError{"Unexpected end of data"};
                }

                return result;
            }

            static std::string parseReference(std::u32string::const_iterator& iterator,
                                              const std::u32string::const_iterator end)
            {
                std::string result;

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                if (*iterator != '&')
                    throw ParseError{"Expected an ampersand"};

                if (++iterator == end)
                    throw ParseError{"Unexpected end of data"};

                std::string value;

                while (*iterator != ';')
                {
                    value += fromUtf32(*iterator);

                    if (++iterator == end)
                        throw ParseError{"Unexpected end of data"};
                }

                ++iterator;

                if (value.empty())
                    throw ParseError{"Invalid entity"};

                if (value[0] == '#') // char reference
                {
                    if (value.length() < 2)
                        throw ParseError{"Invalid entity"};

                    char32_t c = 0;

                    if (value[1] == 'x') // hex value
                    {
                        if (value.length() < 3)
                            throw ParseError{"Invalid entity"};

                        for (std::size_t i = 2; i < value.length(); ++i)
                        {
                            std::uint8_t code = 0;

                            if (value[i] >= '0' && value[i] <= '9')
                                code = static_cast<std::uint8_t>(value[i]) - '0';
                            else if (value[i] >= 'a' && value[i] <='f')
                                code = static_cast<std::uint8_t>(value[i]) - 'a' + 10;
                            else if (value[i] >= 'A' && value[i] <='F')
                                code = static_cast<std::uint8_t>(value[i]) - 'A' + 10;
                            else
                                throw ParseError{"Invalid character code"};

                            c = (c << 4) | code;
                        }
                    }
                    else
                    {
                        for (std::size_t i = 1; i < value.length(); ++i)
                        {
                            const std::uint8_t code = (value[i] >= '0' && value[i] <= '9') ?
                                static_cast<std::uint8_t>(value[i]) - '0' :
                                throw ParseError{"Invalid character code"};

                            c = c * 10 + code;
                        }
                    }

                    result = fromUtf32(c);
                }
                else // entity reference
                {
                    if (value == "quot")
                        result = "\"";
                    else if (value == "amp")
                        result = "&";
                    else if (value == "apos")
                        result = "'";
                    else if (value == "lt")
                        result = "<";
                    else if (value == "gt")
                        result = ">";
                    else
                        throw ParseError{"Invalid entity"};
                }

                return result;
            }

            static std::string parseString(std::u32string::const_iterator& iterator,
                                           const std::u32string::const_iterator end)
            {
                std::string result;

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                if (*iterator != '"' && *iterator != '\'')
                    throw ParseError{"Expected quotes"};

                const auto quotes = *iterator;

                if (++iterator == end)
                    throw ParseError{"Unexpected end of data"};

                while (*iterator != quotes)
                {
                    if (*iterator == '&')
                    {
                        const auto entity = parseReference(iterator, end);
                        result += entity;
                    }
                    else
                    {
                        result += fromUtf32(*iterator);

                        if (++iterator == end)
                            throw ParseError{"Unexpected end of data"};
                    }
                }

                ++iterator;

                return result;
            }

            static Node parseDtdElement(std::u32string::const_iterator& iterator,
                                        const std::u32string::const_iterator end)
            {
                expect(iterator, end, '<');
                expect(iterator, end, '!');

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                Node result;

                const auto type = parseName(iterator, end);

                if (type == "ELEMENT")
                {
                    result = Node::Type::element;
                }
                else if (type == "ATTLIST")
                {
                    result = Node::Type::attributeList;
                }
                else if (type == "ENTITY")
                {
                    result = Node::Type::entity;
                }
                else if (type == "NOTATION")
                {
                    result = Node::Type::notation;
                }

                skipWhitespaces(iterator, end);

                result.setName(parseName(iterator, end));

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                while (*iterator != '>')
                {
                    ++iterator;
                    if (iterator == end)
                        throw ParseError{"Unexpected end of data"};
                }

                ++iterator;

                return result;
            }

            static Node parseElement(std::u32string::const_iterator& iterator,
                                     const std::u32string::const_iterator end,
                                     const bool preserveWhitespaces,
                                     const bool preserveComments,
                                     const bool preserveProcessingInstructions,
                                     const bool prologAllowed)
            {
                expect(iterator, end, '<');

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                Node result;

                if (*iterator == '!') // <!
                {
                    if (++iterator == end)
                        throw ParseError{"Unexpected end of data"};

                    if (*iterator == '-') // <!-
                    {
                        ++iterator;

                        expect(iterator, end, '-'); // <!--

                        result = Node::Type::comment;

                        std::string value;
                        for (;;)
                        {
                            if (std::distance(iterator, end) < 3)
                                throw ParseError{"Unexpected end of data"};

                            if (*iterator == '-')
                            {
                                if (*(iterator + 1) == '-') // --
                                {
                                    iterator += 2;

                                    if (*iterator == '>') // -->
                                    {
                                        ++iterator;
                                        break;
                                    }
                                    else
                                        throw ParseError{"Unexpected double-hyphen inside comment"};
                                }
                            }

                            value += fromUtf32(*iterator);
                            ++iterator;
                        }

                        result.setValue(value);
                    }
                    else if (*iterator == '[') // <![
                    {
                        ++iterator;
                        std::string name;
                        name = parseName(iterator, end);

                        if (name != "CDATA")
                            throw ParseError{"Expected CDATA"};

                        if (iterator == end)
                            throw ParseError{"Unexpected end of data"};

                        expect(iterator, end, '[');

                        result = Node::Type::characterData;

                        std::string value;
                        for (;;)
                        {
                            if (std::distance(iterator, end) < 3)
                                throw ParseError{"Unexpected end of data"};

                            if (*iterator == ']' &&
                                *(iterator + 1) == ']' &&
                                *(iterator + 2) == '>')
                            {
                                iterator += 3;
                                break;
                            }

                            value += fromUtf32(*iterator);
                            ++iterator;
                        }
                        result.setValue(value);
                    }
                    else // <!
                    {
                        std::string type;
                        type = parseName(iterator, end);

                        if (type != "DOCTYPE")
                            throw ParseError{"Invalid document type declaration"};

                        result = Node::Type::documentTypeDefinition;

                        skipWhitespaces(iterator, end);

                        const auto name = parseName(iterator, end);
                        result.setName(name);

                        skipWhitespaces(iterator, end);
                        
                        if (iterator == end)
                            throw ParseError{"Unexpected end of data"};

                        if (*iterator == '[')
                        {
                            if (++iterator == end)
                                throw ParseError{"Unexpected end of data"};

                            skipWhitespaces(iterator, end);

                            if (iterator == end)
                                throw ParseError{"Unexpected end of data"};

                            while (*iterator != ']')
                            {
                                result.pushBack(parseDtdElement(iterator, end));

                                skipWhitespaces(iterator, end);

                                if (iterator == end)
                                    throw ParseError{"Unexpected end of data"};
                            }

                            ++iterator;
                        }
                        else
                        {
                            std::string value;
                            while (*iterator != '>')
                            {
                                value += fromUtf32(*iterator);

                                if (++iterator == end)
                                    throw ParseError{"Unexpected end of data"};
                            }
                            result.setValue(value);
                        }

                        skipWhitespaces(iterator, end);

                        if (iterator == end)
                            throw ParseError{"Unexpected end of data"};

                        if (*iterator != '>')
                            throw ParseError{"Expected a right angle bracket"};

                        ++iterator;
                    }

                }
                else if (*iterator == '?') // <?
                {
                    ++iterator;
                    result = Node::Type::processingInstruction;

                    const auto name = parseName(iterator, end);
                    if (!prologAllowed && name.length() == 3 &&
                        std::tolower(name[0]) == 'x' &&
                        std::tolower(name[1]) == 'm' &&
                        std::tolower(name[2]) == 'l')
                    {
                        throw ParseError{"Invalid processing instruction"};
                    }

                    result.setName(name);

                    skipWhitespaces(iterator, end);

                    if (iterator == end)
                        throw ParseError{"Unexpected end of data"};

                    std::string value;

                    while (*iterator != '?')
                    {
                        value += fromUtf32(*iterator);

                        if (++iterator == end)
                            throw ParseError{"Unexpected end of data"};
                    }

                    if (++iterator == end)
                        throw ParseError{"Unexpected end of data"};

                    expect(iterator, end, '>');

                    result.setValue(value);
                }
                else // <
                {
                    result = Node::Type::tag;
                    result.setName(parseName(iterator, end));

                    bool tagClosed = false;

                    for (;;)
                    {
                        skipWhitespaces(iterator, end);

                        if (iterator == end)
                            throw ParseError{"Unexpected end of data"};

                        if (*iterator == '>')
                        {
                            ++iterator;
                            break;
                        }
                        else if (*iterator == '/')
                        {
                            ++iterator;

                            expect(iterator, end, '>');

                            tagClosed = true;
                            break;
                        }

                        const auto attribute = parseName(iterator, end);

                        skipWhitespaces(iterator, end);

                        expect(iterator, end, '=');

                        skipWhitespaces(iterator, end);

                        result[attribute] = parseString(iterator, end);
                    }

                    if (!tagClosed)
                    {
                        for (;;)
                        {
                            if (!preserveWhitespaces) skipWhitespaces(iterator, end);

                            if (iterator == end)
                                throw ParseError{"Unexpected end of data"};

                            if (*iterator == '<' &&
                                iterator + 1 != end &&
                                *(iterator + 1) == '/')
                            {
                                ++iterator; // skip the left angle bracket
                                ++iterator; // skip the slash

                                if (const auto tag = parseName(iterator, end); tag != result.getName())
                                    throw ParseError{"Tag not closed properly"};

                                expect(iterator, end, '>');
                                break;
                            }
                            else
                            {
                                const auto node = parse(iterator, end,
                                                        preserveWhitespaces,
                                                        preserveComments,
                                                        preserveProcessingInstructions,
                                                        false);

                                if ((preserveComments || node.getType() != Node::Type::comment) &&
                                    (preserveProcessingInstructions || node.getType() != Node::Type::processingInstruction))
                                    result.pushBack(node);
                            }
                        }
                    }
                }

                return result;
            }

            static Node parseText(std::u32string::const_iterator& iterator,
                                  const std::u32string::const_iterator end)
            {
                Node result;
                result = Node::Type::text;

                std::string value;
                for (;;)
                {
                    if (iterator == end || // end of a file
                        *iterator == '<') // start of a tag
                        break;
                    else if (*iterator == '&')
                    {
                        const auto entity = parseReference(iterator, end);
                        value += entity;
                    }
                    else
                    {
                        value += fromUtf32(*iterator);
                        ++iterator;
                    }
                }
                result.setValue(value);
                return result;
            }

            static Node parse(std::u32string::const_iterator& iterator,
                              const std::u32string::const_iterator end,
                              const bool preserveWhitespaces,
                              const bool preserveComments,
                              const bool preserveProcessingInstructions,
                              const bool prologAllowed)
            {

                if (iterator == end)
                    throw ParseError{"Unexpected end of data"};

                if (*iterator == '<')
                    return parseElement(iterator, end,
                                        preserveWhitespaces,
                                        preserveComments,
                                        preserveProcessingInstructions,
                                        prologAllowed);
                else
                    return parseText(iterator, end);
            }
        };

        return Parser::parse(begin, end,
                             preserveWhitespaces,
                             preserveComments,
                             preserveProcessingInstructions);
    }

    inline Data parse(const char* data,
                      const bool preserveWhitespaces = false,
                      const bool preserveComments = false,
                      const bool preserveProcessingInstructions = false)
    {
        auto end = data;
        while (*end) ++end;
        return parse(data, end,
                     preserveWhitespaces,
                     preserveComments,
                     preserveProcessingInstructions);
    }

    template <class T>
    Data parse(const T& data,
               const bool preserveWhitespaces = false,
               const bool preserveComments = false,
               const bool preserveProcessingInstructions = false)
    {
        using std::begin, std::end; // add std::begin and std::end to lookup
        return parse(begin(data), end(data),
                     preserveWhitespaces,
                     preserveComments,
                     preserveProcessingInstructions);
    }

    inline std::string encode(const Data& data,
                              const bool whitespaces = false,
                              const bool byteOrderMark = false)
    {
        class Encoder final
        {
        public:
            static std::string encode(const Data& data,
                                      const bool whitespaces,
                                      const bool byteOrderMark)
            {
                std::string result;
                if (byteOrderMark) result.assign(utf8ByteOrderMark.begin(),
                                                 utf8ByteOrderMark.end());

                for (const Node& node : data)
                {
                    encode(node, result, whitespaces);
                    if (whitespaces) result.push_back('\n');
                }

                return result;
            }

        private:
            static void encode(const std::string& str,
                               std::string& result)
            {
                for (const char c : str)
                {
                    switch (c)
                    {
                        case '"':
                            result.insert(result.end(), {'&', 'q', 'u', 'o', 't', ';'});
                            break;
                        case '&':
                            result.insert(result.end(), {'&', 'a', 'm', 'p', ';'});
                            break;
                        case '\'':
                            result.insert(result.end(), {'&', 'a', 'p', 'o', 's', ';'});
                            break;
                        case '<':
                            result.insert(result.end(), {'&', 'l', 't', ';'});
                            break;
                        case '>':
                            result.insert(result.end(), {'&', 'g', 't', ';'});
                            break;
                        default:
                            result.insert(result.end(), c);
                            break;
                    }
                }
            }

            static void encode(const Node& node, std::string& result,
                               const bool whitespaces,
                               const std::size_t level = 0)
            {
                switch (node.getType())
                {
                    case Node::Type::comment:
                    {
                        const auto& value = node.getValue();
                        result.insert(result.end(), {'<', '!', '-', '-'});
                        result.insert(result.end(), value.begin(), value.end());
                        result.insert(result.end(), {'-', '-', '>'});
                        break;
                    }
                    case Node::Type::characterData:
                    {
                        const auto& value = node.getValue();
                        result.insert(result.end(), {'<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['});
                        result.insert(result.end(), value.begin(), value.end());
                        result.insert(result.end(), {']', ']', '>'});
                        break;
                    }
                    case Node::Type::processingInstruction:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), {'<', '?'});
                        result.insert(result.end(), name.begin(), name.end());

                        const auto& value = node.getValue();
                        result.insert(result.end(), ' ');
                        result.insert(result.end(), value.begin(), value.end());
                        result.insert(result.end(), {'?', '>'});
                        break;
                    }
                    case Node::Type::documentTypeDefinition:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), {'<', '!', 'D', 'O', 'C', 'T', 'Y', 'P', 'E', ' '});
                        result.insert(result.end(), name.begin(), name.end());

                        switch (node.getExternalIdType())
                        {
                            case Node::ExternalIdType::none: break;
                            case Node::ExternalIdType::system:
                                result.insert(result.end(), {' ', 'S', 'Y', 'S', 'T', 'E', 'M'});
                                break;
                            case Node::ExternalIdType::pub:
                                result.insert(result.end(), {' ', 'P', 'U', 'B', 'L', 'I', 'C'});
                                break;
                        }

                        const auto& value = node.getValue();
                        if (!value.empty())
                        {
                            result.insert(result.end(), ' ');
                            result.insert(result.end(), value.begin(), value.end());
                        }

                        if (const auto& children = node.getChildren(); !children.empty())
                        {
                            result.insert(result.end(), {' ', '['});
                            if (whitespaces) result.push_back('\n');

                            for (const Node& child : children)
                            {
                                if (whitespaces) result.insert(result.end(), level + 1, '\t');
                                encode(child, result, whitespaces, level + 1);
                                if (whitespaces) result.push_back('\n');
                            }

                            if (whitespaces) result.insert(result.end(), level, '\t');
                            result.insert(result.end(), ']');
                        }

                        result.insert(result.end(), '>');

                        break;
                    }
                    case Node::Type::element:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), {'<', '!', 'E', 'L', 'E', 'M', 'E', 'N', 'T', ' '});
                        result.insert(result.end(), name.begin(), name.end());

                        const auto& value = node.getValue();
                        if (!value.empty())
                        {
                            result.insert(result.end(), ' ');
                            result.insert(result.end(), value.begin(), value.end());
                        }
                        result.insert(result.end(), '>');
                        break;
                    }
                    case Node::Type::attributeList:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), {'<', '!', 'A', 'T', 'T', 'L', 'I', 'S', 'T', ' '});
                        result.insert(result.end(), name.begin(), name.end());

                        const auto& value = node.getValue();
                        if (!value.empty())
                        {
                            result.insert(result.end(), ' ');
                            result.insert(result.end(), value.begin(), value.end());
                        }
                        result.insert(result.end(), '>');
                        break;
                    }
                    case Node::Type::entity:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), {'<', '!', 'E', 'N', 'T', 'I', 'T', 'Y', ' '});
                        result.insert(result.end(), name.begin(), name.end());

                        const auto& value = node.getValue();
                        if (!value.empty())
                        {
                            result.insert(result.end(), ' ');
                            result.insert(result.end(), value.begin(), value.end());
                        }
                        result.insert(result.end(), '>');
                        break;
                    }
                    case Node::Type::notation:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), {'<', '!', 'N', 'O', 'T', 'A', 'T', 'I', 'O', 'N', ' '});
                        result.insert(result.end(), name.begin(), name.end());

                        const auto& value = node.getValue();
                        if (!value.empty())
                        {
                            result.insert(result.end(), ' ');
                            result.insert(result.end(), value.begin(), value.end());
                        }
                        result.insert(result.end(), '>');
                        break;
                    }
                    case Node::Type::tag:
                    {
                        const auto& name = node.getName();
                        result.insert(result.end(), '<');
                        result.insert(result.end(), name.begin(), name.end());

                        const auto& attributes = node.getAttributes();
                        for (const auto& [key, attributeValue] : attributes)
                        {
                            result.insert(result.end(), ' ');
                            result.insert(result.end(), key.begin(), key.end());
                            result.insert(result.end(), {'=', '"'});
                            result.insert(result.end(), attributeValue.begin(), attributeValue.end());
                            result.insert(result.end(), '"');
                        }

                        if (const auto& children = node.getChildren(); !children.empty())
                        {
                            result.insert(result.end(), '>');
                            if (whitespaces) result.push_back('\n');

                            for (const Node& child : children)
                            {
                                if (whitespaces) result.insert(result.end(), level + 1, '\t');
                                encode(child, result, whitespaces, level + 1);
                                if (whitespaces) result.push_back('\n');
                            }

                            if (whitespaces) result.insert(result.end(), level, '\t');
                            result.insert(result.end(), {'<', '/'});
                            result.insert(result.end(), name.begin(), name.end());
                            result.insert(result.end(), '>');
                        }
                        else
                            result.insert(result.end(), {'/', '>'});
                        break;
                    }
                    case Node::Type::text:
                    {
                        const auto& value = node.getValue();
                        encode(value, result);
                        break;
                    }
                    default:
                        throw ParseError{"Unknown node type"};
                }
            }
        };

        return Encoder::encode(data, whitespaces, byteOrderMark);
    }
}

#endif // OUZEL_FORMATS_XML_HPP
