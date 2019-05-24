// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Localization.hpp"

static constexpr uint32_t MAGIC_BIG = 0xde120495;
static constexpr uint32_t MAGIC_LITTLE = 0x950412de;

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

        uint32_t magic = *reinterpret_cast<const uint32_t*>(data.data() + offset);
        offset += sizeof(magic);

        std::function<uint32_t(const uint8_t*)> decodeUInt32;

        if (magic == MAGIC_BIG)
            decodeUInt32 = [](const uint8_t* bytes) {
                return static_cast<uint32_t>(bytes[3] |
                                             (bytes[2] << 8) |
                                             (bytes[1] << 16) |
                                             (bytes[0] << 24));
            };
        else if (magic == MAGIC_LITTLE)
            decodeUInt32 = [](const uint8_t* bytes) {
                return static_cast<uint32_t>(bytes[0] |
                                             (bytes[1] << 8) |
                                             (bytes[2] << 16) |
                                             (bytes[3] << 24));
            };
        else
            throw std::runtime_error("Wrong magic " + std::to_string(magic));

        uint32_t revision = decodeUInt32(data.data() + offset);
        offset += sizeof(revision);

        if (revision != 0)
            throw std::runtime_error("Unsupported revision " + std::to_string(revision));

        uint32_t stringCount = decodeUInt32(data.data() + offset);
        offset += sizeof(stringCount);

        std::vector<TranslationInfo> translations(stringCount);

        uint32_t stringsOffset = decodeUInt32(data.data() + offset);
        offset += sizeof(stringsOffset);

        uint32_t translationsOffset = decodeUInt32(data.data() + offset);
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
        std::shared_ptr<Language> language = std::make_shared<Language>(data);
        languages[name] = language;
    }

    void Localization::setLanguage(const std::string& language)
    {
        auto i = languages.find(language);

        if (i != languages.end())
            currentLanguage = i->second;
        else
            currentLanguage.reset();
    }

    std::string Localization::getString(const std::string& str) const
    {
        if (currentLanguage)
            return currentLanguage->getString(str);
        else
            return str;
    }
}
