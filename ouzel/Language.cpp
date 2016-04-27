// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include "Language.h"
#include "Engine.h"
#include "Utils.h"

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
        std::shared_ptr<Language> result = std::make_shared<Language>();

        if (!result->initFromFile(filename))
        {
            result.reset();
        }

        return result;
    }

    bool Language::initFromFile(const std::string& filename)
    {
        std::ifstream file(sharedEngine->getFileSystem()->getPath(filename), std::ios::binary);
        file >> std::noskipws;

        if (!file)
        {
            log("Failed to open file %s", filename.c_str());
            return false;
        }

        uint32_t magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));

        if (magic != 0x950412DE)
        {
            log("Wrong magic %x", magic);
            return false;
        }

        uint32_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));

        uint32_t stringCount;
        file.read(reinterpret_cast<char*>(&stringCount), sizeof(stringCount));

        std::vector<TranslationInfo> translations(stringCount);

        uint32_t stringsOffset;
        file.read(reinterpret_cast<char*>(&stringsOffset), sizeof(stringsOffset));

        uint32_t translationsOffset;
        file.read(reinterpret_cast<char*>(&translationsOffset), sizeof(translationsOffset));

        file.seekg(stringsOffset);

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            file.read(reinterpret_cast<char*>(&translations[i].stringLength), sizeof(translations[i].stringLength));
            file.read(reinterpret_cast<char*>(&translations[i].stringOffset), sizeof(translations[i].stringOffset));
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            file.read(reinterpret_cast<char*>(&translations[i].translationLength), sizeof(translations[i].translationLength));
            file.read(reinterpret_cast<char*>(&translations[i].translationOffset), sizeof(translations[i].translationOffset));
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            file.seekg(translations[i].stringOffset);
            std::string str;

            std::istream_iterator<char> isi(file);
            std::copy_n(isi, translations[i].stringLength, std::insert_iterator<std::string>(str, str.begin()));

            file.seekg(translations[i].translationOffset);
            std::string translation;

            isi = std::istream_iterator<char>(file);
            std::copy_n(isi, translations[i].translationLength, std::insert_iterator<std::string>(translation, translation.begin()));

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
