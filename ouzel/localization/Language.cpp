// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <vector>
#include "Language.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    struct TranslationInfo
    {
        uint32_t stringLength;
        uint32_t stringOffset;

        uint32_t translationLength;
        uint32_t translationOffset;
    };

    LanguagePtr Language::createFromFile(const std::string& filename)
    {
        LanguagePtr result = std::make_shared<Language>();

        if (!result->initFromFile(filename))
        {
            result.reset();
        }

        return result;
    }

    bool Language::initFromFile(const std::string& filename)
    {
        const unsigned long MAGIC_BIG = 0xde120495;
        const unsigned long MAGIC_LITTLE = 0x950412de;

        std::vector<uint8_t> data;

        if (!sharedApplication->getFileSystem()->loadFile(filename, data))
        {
            return false;
        }

        uint32_t offset = 0;

        if (data.size() < 5 * sizeof(uint32_t))
        {
            return false;
        }

        uint32_t magic = *reinterpret_cast<uint32_t*>(data.data() + offset);
        offset += sizeof(magic);

        uint32_t (*readUInt32)(const uint8_t*) = nullptr;

        if (magic == MAGIC_BIG)
        {
            readUInt32 = readUInt32Big;
        }
        else if (magic == MAGIC_LITTLE)
        {
            readUInt32 = readUInt32Little;
        }
        else
        {
            Log(Log::Level::ERR) << "Wrong magic " << magic;
            return false;
        }

        uint32_t revision = readUInt32(data.data() + offset);
        offset += sizeof(revision);

        if (revision != 0)
        {
            Log(Log::Level::ERR) << "Unsupported revision " << revision;
            return false;
        }

        uint32_t stringCount = readUInt32(data.data() + offset);
        offset += sizeof(stringCount);

        std::vector<TranslationInfo> translations(stringCount);

        uint32_t stringsOffset = readUInt32(data.data() + offset);
        offset += sizeof(stringsOffset);

        uint32_t translationsOffset = readUInt32(data.data() + offset);
        offset += sizeof(translationsOffset);

        offset = stringsOffset;

        if (data.size() < offset + 2 * sizeof(uint32_t) * stringCount)
        {
            return false;
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            translations[i].stringLength = readUInt32(data.data() + offset);
            offset += sizeof(translations[i].stringLength);

            translations[i].stringOffset = readUInt32(data.data() + offset);
            offset += sizeof(translations[i].stringOffset);
        }

        offset = translationsOffset;

        if (data.size() < offset + 2 * sizeof(uint32_t) * stringCount)
        {
            return false;
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            translations[i].translationLength = readUInt32(data.data() + offset);
            offset += sizeof(translations[i].translationLength);

            translations[i].translationOffset = readUInt32(data.data() + offset);
            offset += sizeof(translations[i].translationOffset);
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            if (data.size() < translations[i].stringOffset + translations[i].stringLength ||
                data.size() < translations[i].translationOffset + translations[i].translationLength)
            {
                return false;
            }

            std::string str(reinterpret_cast<char*>(data.data() + translations[i].stringOffset), translations[i].stringLength);
            std::string translation(reinterpret_cast<char*>(data.data() + translations[i].translationOffset), translations[i].translationLength);

            strings[str] = translation;
        }

        return true;
    }

    std::string Language::getString(const std::string& str)
    {
        auto i = strings.find(str);

        if (i != strings.end())
        {
            return i->second;
        }
        else
        {
            return str;
        }
    }
}
