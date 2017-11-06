// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cctype>
#include <algorithm>
#include <iterator>
#include "INI.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace ini
    {
        Section::Section()
        {
        }

        Section::Section(const std::string& aName):
            name(aName)
        {
        }

        bool Section::hasValue(const std::string& key) const
        {
            auto valueIterator = values.find(key);
            return valueIterator != values.end();
        }

        const std::string& Section::getValue(const std::string& key)
        {
            return values[key];
        }

        std::string Section::getValue(const std::string& key) const
        {
            auto valueIterator = values.find(key);
            if (valueIterator != values.end())
            {
                return valueIterator->second;
            }

            return std::string();
        }

        const std::string& Section::getValue(const std::string& key, const std::string& defaultValue) const
        {
            auto valueIterator = values.find(key);

            if (valueIterator != values.end())
            {
                return valueIterator->second;
            }

            return defaultValue;
        }

        void Section::setValue(const std::string& key, const std::string& value)
        {
            values[key] = value;
        }

        bool Section::deleteValue(const std::string& key)
        {
            auto valueIterator = values.find(key);

            if (valueIterator != values.end())
            {
                values.erase(valueIterator);
                return true;
            }

            return false;
        }

        Data::Data()
        {
        }

        Data::Data(const std::string& filename)
        {
            init(filename);
        }

        Data::Data(const std::vector<uint8_t>& data)
        {
            init(data);
        }

        bool Data::init(const std::string& filename)
        {
            std::vector<uint8_t> data;

            if (!engine->getFileSystem()->readFile(filename, data))
            {
                return false;
            }

            return init(data);
        }

        static inline std::vector<uint32_t>& ltrimUtf32(std::vector<uint32_t>& s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                            std::not1(std::ptr_fun<int, int>(std::isspace))));
            return s;
        }

        static inline std::vector<uint32_t>& rtrimUtf32(std::vector<uint32_t>& s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(),
                                 std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
            return s;
        }

        static inline std::vector<uint32_t>& trimUtf32(std::vector<uint32_t>& s)
        {
            return ltrimUtf32(rtrimUtf32(s));
        }

        bool Data::init(const std::vector<uint8_t>& data)
        {
            std::vector<uint32_t> utf32;

            // BOM
            if (data.size() >= 3 &&
                data[0] == 0xEF &&
                data[1] == 0xBB &&
                data[2] == 0xBF)
            {
                std::vector<uint8_t> newData(data.begin() + 3, data.end());

                utf32 = utf8to32(newData);
            }
            else
            {
                utf32 = utf8to32(data);
            }

            Section* section = &sections[""]; // default section

            for (auto i = utf32.begin(); i != utf32.end();)
            {
                if (*i == '\n' || *i == '\r' || *i == ' ' || *i == '\t') // line starts with a whitespace
                {
                    ++i; // skip the white space
                }
                else if (*i == '[') // section
                {
                    ++i; // skip the left bracket

                    std::vector<uint32_t> sectionUtf32;
                    bool parsedSection = false;

                    for (;;)
                    {
                        if (i == utf32.end() || *i == '\r' || *i == '\n')
                        {
                            if (!parsedSection)
                            {
                                Log(Log::Level::ERR) << "Unexpected end of section";
                                return false;
                            }
                            ++i; // skip the newline
                            break;
                        }
                        else if (*i == ';')
                        {
                            ++i; // skip the semicolon

                            if (!parsedSection)
                            {
                                Log(Log::Level::ERR) << "Unexpected comment";
                                return false;
                            }

                            for (;;)
                            {
                                if (i == utf32.end() || *i == '\r' || *i == '\n')
                                {
                                    ++i; // skip the newline
                                    break;
                                }

                                ++i;
                            }
                            break;
                        }
                        else if (*i == ']')
                        {
                            parsedSection = true;
                        }
                        else if (*i != ' ' && *i != '\t')
                        {
                            if (parsedSection)
                            {
                                Log(Log::Level::ERR) << "Unexpected character after section";
                                return false;
                            }
                        }

                        if (!parsedSection)
                        {
                            sectionUtf32.push_back(*i);
                        }

                        ++i;
                    }

                    if (sectionUtf32.empty())
                    {
                        Log(Log::Level::ERR) << "Invalid section name";
                        return false;
                    }

                    std::string sectionName = utf32to8(sectionUtf32);

                    section = &sections[sectionName];
                }
                else if (*i == ';') // comment
                {
                    ++i; // skip the semicolon

                    while (i != utf32.end())
                    {
                        if (*i == '\r' || *i == '\n')
                        {
                            ++i; // skip the newline
                            break;
                        }

                        ++i;
                    }
                }
                else // key, value pair
                {
                    std::vector<uint32_t> keyUtf32;
                    std::vector<uint32_t> valueUtf32;
                    bool parsedKey = false;

                    for (;;)
                    {
                        if (i == utf32.end())
                        {
                            break;
                        }
                        else if (*i == '\r' || *i == '\n')
                        {
                            ++i; // skip the newline
                            break;
                        }
                        else if (*i == '=')
                        {
                            if (!parsedKey)
                            {
                                parsedKey = true;
                            }
                            else
                            {
                                Log(Log::Level::ERR) << "Unexpected character";
                                return false;
                            }
                        }
                        else if (*i == ';')
                        {
                            ++i; // skip the semicolon

                            for (;;)
                            {
                                if (i == utf32.end() || *i == '\r' || *i == '\n')
                                {
                                    ++i; // skip the newline
                                    break;
                                }

                                ++i;
                            }
                            break;
                        }
                        else
                        {
                            if (!parsedKey)
                            {
                                keyUtf32.push_back(*i);
                            }
                            else
                            {
                                valueUtf32.push_back(*i);
                            }
                        }

                        ++i;
                    }

                    if (keyUtf32.empty())
                    {
                        Log(Log::Level::ERR) << "Invalid key name";
                        return false;
                    }

                    keyUtf32 = trimUtf32(keyUtf32);
                    valueUtf32 = trimUtf32(valueUtf32);

                    std::string key = utf32to8(keyUtf32);
                    std::string value = utf32to8(valueUtf32);

                    section->values[key] = value;
                }
            }

            return true;
        }

        bool Data::save(const std::string& filename)
        {
            std::vector<uint8_t> data;
            encode(data);

            return engine->getFileSystem()->writeFile(filename, data);
        }

        bool Data::encode(std::vector<uint8_t>& data)
        {
            std::string result;

            for (const auto& section : sections)
            {
                data.push_back('[');
                std::copy(section.first.begin(), section.first.end(), std::back_inserter(data));
                data.push_back(']');
                data.push_back('\n');

                for (const auto& value : section.second.values)
                {
                    std::copy(value.first.begin(), value.first.end(), std::back_inserter(data));
                    data.push_back('=');
                    std::copy(value.second.begin(), value.second.end(), std::back_inserter(data));
                    data.push_back('\n');
                }
            }

            return true;
        }

        bool Data::hasSection(const std::string& section) const
        {
            auto sectionIterator = sections.find(section);
            return sectionIterator != sections.end();
        }

        const Section& Data::getSection(const std::string& name)
        {
            return sections[name];
        }

        Section Data::getSection(const std::string& name) const
        {
            auto sectionIterator = sections.find(name);
            if (sectionIterator != sections.end())
            {
                return sectionIterator->second;
            }

            return Section();
        }

        void Data::setSection(const std::string& name, const Section& section)
        {
            sections[name] = section;
        }

        bool Data::deleteSection(const std::string& name)
        {
            auto sectionIterator = sections.find(name);
            if (sectionIterator != sections.end())
            {
                sections.erase(sectionIterator);
                return true;
            }

            return false;
        }
    }
}
