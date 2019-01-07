// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Localization.hpp"
#include "Language.hpp"

namespace ouzel
{
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
