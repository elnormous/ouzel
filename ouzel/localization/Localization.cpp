// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Localization.h"
#include "Language.h"

namespace ouzel
{
    void Localization::addLanguage(const std::string& name, const std::string& filename)
    {
        std::shared_ptr<Language> language = std::make_shared<Language>();

        if (language->initFromFile(filename))
        {
            languages[name] = language;
        }
    }

    void Localization::setLanguage(const std::string& language)
    {
        auto i = languages.find(language);

        if (i != languages.end())
        {
            currentLanguage = i->second;
        }
        else
        {
            currentLanguage.reset();
        }
    }

    std::string Localization::getString(const std::string& str)
    {
        if (currentLanguage)
        {
            return currentLanguage->getString(str);
        }
        else
        {
            return str;
        }
    }
}
