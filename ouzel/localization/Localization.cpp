// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Localization.hpp"
#include "Language.hpp"

namespace ouzel
{
    void Localization::addLanguage(const std::string& name, const std::string& filename)
    {
        std::shared_ptr<Language> language = std::make_shared<Language>();
        language->init(filename);
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

    std::string Localization::getString(const std::string& str)
    {
        if (currentLanguage)
            return currentLanguage->getString(str);
        else
            return str;
    }
}
