// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_INI_HPP
#define OUZEL_UTILS_INI_HPP

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ouzel
{
    namespace ini
    {
        inline namespace detail
        {
            constexpr std::uint8_t UTF8_BOM[] = {0xEF, 0xBB, 0xBF};
        }

        class ParseError final: public std::logic_error
        {
        public:
            explicit ParseError(const std::string& str): std::logic_error(str) {}
            explicit ParseError(const char* str): std::logic_error(str) {}
        };
        
        class Data;

        class Section final
        {
            friend Data;
        public:
            Section() = default;

            explicit Section(const std::string& initName):
                name(initName)
            {
            }

            inline auto& getName() const noexcept { return name; }
            inline void setName(const std::string& newName) { name = newName; }

            inline auto& getValues() const noexcept { return values; }

            inline bool hasValue(const std::string& key) const
            {
                auto valueIterator = values.find(key);
                return valueIterator != values.end();
            }

            auto& getValue(const std::string& key)
            {
                return values[key];
            }

            auto getValue(const std::string& key) const
            {
                auto valueIterator = values.find(key);
                if (valueIterator != values.end())
                    return valueIterator->second;

                return std::string();
            }

            auto& getValue(const std::string& key, const std::string& defaultValue) const
            {
                auto valueIterator = values.find(key);

                if (valueIterator != values.end())
                    return valueIterator->second;

                return defaultValue;
            }

            void setValue(const std::string& key, const std::string& value)
            {
                values[key] = value;
            }

            void deleteValue(const std::string& key)
            {
                auto valueIterator = values.find(key);

                if (valueIterator != values.end())
                    values.erase(valueIterator);
            }

        private:
            void encode(std::vector<std::uint8_t>& data) const
            {
                if (!name.empty())
                {
                    data.push_back('[');
                    data.insert(data.end(), name.begin(), name.end());
                    data.push_back(']');
                    data.push_back('\n');
                }

                for (const auto& value : values)
                {
                    data.insert(data.end(), value.first.begin(), value.first.end());
                    data.push_back('=');
                    data.insert(data.end(), value.second.begin(), value.second.end());
                    data.push_back('\n');
                }
            }

            std::string name;
            std::map<std::string, std::string> values;
        };

        template <typename T>
        constexpr auto isWhitespace(const T c) noexcept
        {
            return c == ' ' || c == '\t';
        }

        template <class T>
        inline T& leftTrim(T& s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                            [](auto c) noexcept {return !isWhitespace(c);}));
            return s;
        }

        template <class T>
        inline T& rightTrim(T& s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(),
                                 [](auto c) noexcept {return !isWhitespace(c);}).base(), s.end());
            return s;
        }

        template <class T>
        inline T& trim(T& s)
        {
            return leftTrim(rightTrim(s));
        }

        class Data final
        {
        public:
            Data() = default;

            template <class T>
            explicit Data(const T& data):
                Data(std::begin(data), std::end(data))
            {}

            template <class Iterator>
            Data(Iterator begin, Iterator end)
            {
                byteOrderMark = std::distance(begin, end) >= 3 &&
                    std::equal(begin, begin + 3,
                               std::begin(UTF8_BOM));

                parse(byteOrderMark ? begin + 3 : begin, end);
            }

            std::vector<std::uint8_t> encode() const
            {
                std::vector<std::uint8_t> result;

                if (byteOrderMark) result.assign(std::begin(UTF8_BOM), std::end(UTF8_BOM));

                auto i = sections.find("");
                if (i != sections.end())
                    i->second.encode(result);

                for (const auto& section : sections)
                {
                    if (!section.first.empty())
                        section.second.encode(result);
                }

                return result;
            }

            inline auto& getSections() const noexcept { return sections; }

            inline bool hasSection(const std::string& name) const
            {
                auto sectionIterator = sections.find(name);
                return sectionIterator != sections.end();
            }

            inline auto& getSection(const std::string& name)
            {
                return sections[name];
            }

            auto getSection(const std::string& name) const
            {
                auto sectionIterator = sections.find(name);
                if (sectionIterator != sections.end())
                    return sectionIterator->second;

                return Section();
            }

            inline void setSection(const std::string& name, const Section& section)
            {
                sections[name] = section;
            }

            void deleteSection(const std::string& name)
            {
                auto sectionIterator = sections.find(name);
                if (sectionIterator != sections.end())
                    sections.erase(sectionIterator);
            }

            bool hasByteOrderMark() const noexcept { return byteOrderMark; }
            void setByteOrderMark(const bool newByteOrderMark) noexcept { byteOrderMark = newByteOrderMark; }

        private:
            template <class Iterator>
            void parse(Iterator begin, Iterator end)
            {
                std::map<std::string, Section>::iterator sectionIterator;
                std::tie(sectionIterator, std::ignore) = sections.insert(std::make_pair("", Section())); // default section

                for (auto iterator = begin; iterator != end;)
                {
                    if (isWhitespace(*iterator) || *iterator == '\n' || *iterator == '\r') // line starts with a whitespace
                        ++iterator; // skip the white space
                    else if (*iterator == '[') // section
                    {
                        ++iterator; // skip the left bracket

                        std::string section;
                        bool parsedSection = false;

                        for (;;)
                        {
                            if (iterator == end || *iterator == '\n' || *iterator == '\r')
                            {
                                if (!parsedSection)
                                    throw ParseError("Unexpected end of section");

                                ++iterator; // skip the newline
                                break;
                            }
                            else if (*iterator == ';')
                            {
                                ++iterator; // skip the semicolon

                                if (!parsedSection)
                                    throw ParseError("Unexpected comment");

                                while (iterator != end)
                                {
                                    if (*iterator == '\n' || *iterator == '\r')
                                    {
                                        ++iterator; // skip the newline
                                        break;
                                    }

                                    ++iterator;
                                }
                                break;
                            }
                            else if (*iterator == ']')
                                parsedSection = true;
                            else if (*iterator != ' ' && *iterator != '\t')
                            {
                                if (parsedSection)
                                    throw ParseError("Unexpected character after section");
                            }

                            if (!parsedSection)
                                section.push_back(static_cast<char>(*iterator));

                            ++iterator;
                        }

                        trim(section);

                        if (section.empty())
                            throw ParseError("Invalid section name");

                        std::tie(sectionIterator, std::ignore) = sections.insert(std::make_pair(section, Section(section)));
                    }
                    else if (*iterator == ';') // comment
                    {
                        while (++iterator != end)
                        {
                            if (*iterator == '\r' || *iterator == '\n')
                            {
                                ++iterator; // skip the newline
                                break;
                            }
                        }
                    }
                    else // key, value pair
                    {
                        std::string key;
                        std::string value;
                        bool parsedKey = false;

                        while (iterator != end)
                        {
                            if (*iterator == '\r' || *iterator == '\n')
                            {
                                ++iterator; // skip the newline
                                break;
                            }
                            else if (*iterator == '=')
                            {
                                if (!parsedKey)
                                    parsedKey = true;
                                else
                                    throw ParseError("Unexpected character");
                            }
                            else if (*iterator == ';')
                            {
                                ++iterator; // skip the semicolon

                                while (iterator != end)
                                {
                                    if (*iterator == '\r' || *iterator == '\n')
                                    {
                                        ++iterator; // skip the newline
                                        break;
                                    }

                                    ++iterator;
                                }
                                break;
                            }
                            else
                            {
                                if (!parsedKey)
                                    key.push_back(static_cast<char>(*iterator));
                                else
                                    value.push_back(static_cast<char>(*iterator));
                            }

                            ++iterator;
                        }

                        if (key.empty())
                            throw ParseError("Invalid key name");

                        trim(key);
                        trim(value);

                        sectionIterator->second.values[key] = value;
                    }
                }
            }

            bool byteOrderMark = false;
            std::map<std::string, Section> sections;
        };
    } // namespace ini
} // namespace ouzel

#endif // OUZEL_UTILS_INI_HPP
