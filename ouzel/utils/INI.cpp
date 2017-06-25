// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "INI.h"
#include "core/Application.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    INI::INI()
    {
    }

    INI::INI(const std::string& filename)
    {
        init(filename);
    }

    INI::INI(const std::vector<uint8_t>& data)
    {
        init(data);
    }

    bool INI::init(const std::string& filename)
    {
        std::vector<uint8_t> data;

        if (!sharedApplication->getFileSystem()->readFile(filename, data))
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

    bool INI::init(const std::vector<uint8_t>& data)
    {
        std::vector<uint32_t> utf32 = utf8to32(data);

        std::string section;

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

                section = utf32to8(sectionUtf32);
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
                if (section.empty())
                {
                    Log(Log::Level::ERR) << "No section";
                    return false;
                }

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

                sections[section][key] = value;
            }
        }

        return true;
    }

    std::string INI::getValue(const std::string& section, const std::string& key)
    {
        auto sectionIterator = sections.find(section);
        if (sectionIterator != sections.end())
        {
            auto& values = sectionIterator->second;

            auto valueIterator = values.find(key);

            if (valueIterator != values.end())
            {
                return valueIterator->second;
            }
        }

        return "";
    }

    void INI::setValue(const std::string& section, const std::string& key, const std::string& value)
    {
        sections[section][key] = value;
    }

    bool INI::deleteValue(const std::string& section, const std::string& key)
    {
        auto sectionIterator = sections.find(section);
        if (sectionIterator != sections.end())
        {
            auto& values = sectionIterator->second;

            auto valueIterator = values.find(key);

            if (valueIterator != values.end())
            {
                values.erase(valueIterator);
                return true;
            }
        }

        return false;
    }
}
