// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Localization.hpp"

namespace ouzel
{
    struct TranslationInfo final
    {
        uint32_t stringLength;
        uint32_t stringOffset;

        uint32_t translationLength;
        uint32_t translationOffset;
    };

    Language::Language(const std::vector<uint8_t>& data)
    {
        uint32_t offset = 0;

        if (data.size() < 5 * sizeof(uint32_t))
            throw std::runtime_error("Not enough data");

        const uint32_t magic = static_cast<uint32_t>(data[0] |
                                                     (data[1] << 8) |
                                                     (data[2] << 16) |
                                                     (data[3] << 24));
        offset += sizeof(magic);

        const auto decodeUInt32 = [magic]() -> std::function<uint32_t(const uint8_t*)> {
            constexpr uint32_t MAGIC_BIG = 0xDE120495;
            constexpr uint32_t MAGIC_LITTLE = 0x950412DE;

            if (magic == MAGIC_BIG)
                return [](const uint8_t* bytes) {
                    return static_cast<uint32_t>(bytes[3] |
                                                 (bytes[2] << 8) |
                                                 (bytes[1] << 16) |
                                                 (bytes[0] << 24));
                };
            else if (magic == MAGIC_LITTLE)
                return [](const uint8_t* bytes) {
                    return static_cast<uint32_t>(bytes[0] |
                                                 (bytes[1] << 8) |
                                                 (bytes[2] << 16) |
                                                 (bytes[3] << 24));
                };
            else
                throw std::runtime_error("Wrong magic " + std::to_string(magic));
        }();

        const uint32_t revision = decodeUInt32(data.data() + offset);
        offset += sizeof(revision);

        if (revision != 0)
            throw std::runtime_error("Unsupported revision " + std::to_string(revision));

        const uint32_t stringCount = decodeUInt32(data.data() + offset);
        offset += sizeof(stringCount);

        std::vector<TranslationInfo> translations(stringCount);

        const uint32_t stringsOffset = decodeUInt32(data.data() + offset);
        offset += sizeof(stringsOffset);

        const uint32_t translationsOffset = decodeUInt32(data.data() + offset);
        offset += sizeof(translationsOffset);

        offset = stringsOffset;

        if (data.size() < offset + 2 * sizeof(uint32_t) * stringCount)
            throw std::runtime_error("Not enough data");

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            translations[i].stringLength = decodeUInt32(data.data() + offset);
            offset += sizeof(translations[i].stringLength);

            translations[i].stringOffset = decodeUInt32(data.data() + offset);
            offset += sizeof(translations[i].stringOffset);
        }

        offset = translationsOffset;

        if (data.size() < offset + 2 * sizeof(uint32_t) * stringCount)
            throw std::runtime_error("Not enough data");

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            translations[i].translationLength = decodeUInt32(data.data() + offset);
            offset += sizeof(translations[i].translationLength);

            translations[i].translationOffset = decodeUInt32(data.data() + offset);
            offset += sizeof(translations[i].translationOffset);
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            if (data.size() < translations[i].stringOffset + translations[i].stringLength ||
                data.size() < translations[i].translationOffset + translations[i].translationLength)
                throw std::runtime_error("Not enough data");

            std::string str(reinterpret_cast<const char*>(data.data() + translations[i].stringOffset), translations[i].stringLength);
            std::string translation(reinterpret_cast<const char*>(data.data() + translations[i].translationOffset), translations[i].translationLength);

            strings[str] = translation;
        }
    }

    std::string Language::getString(const std::string& str) const
    {
        auto i = strings.find(str);

        if (i != strings.end())
            return i->second;
        else
            return str;
    }
    
    void Localization::addLanguage(const std::string& name, const std::vector<uint8_t>& data)
    {
        auto i = languages.find(name);

        if (i != languages.end())
            i->second = Language(data);
        else
            languages.insert(std::make_pair(name, Language(data)));
    }

    void Localization::removeLanguage(const std::string& name)
    {
        auto i = languages.find(name);

        if (i != languages.end())
        {
            if (currentLanguage == i)
                currentLanguage = languages.end();

            languages.erase(i);
        }
    }

    void Localization::setLanguage(const std::string& name)
    {
        auto i = languages.find(name);

        if (i != languages.end())
            currentLanguage = i;
        else
            currentLanguage = languages.end();
    }

    std::string Localization::getString(const std::string& str) const
    {
        if (currentLanguage != languages.end())
            return currentLanguage->second.getString(str);
        else
            return str;
    }
}
