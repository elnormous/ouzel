// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Localization.hpp"
#include "Language.hpp"

namespace ouzel
{
    void Localization::addLanguage(const std::string& name, const std::vector<uint8_t>& data)
    {
        std::unique_ptr<Language> language(new Language(data));
        languages[name] = std::move(language);
    }

    void Localization::setLanguage(const std::string& language)
    {
        auto i = languages.find(language);

        if (i != languages.end())
            currentLanguage = i->second.get();
        else
            currentLanguage = nullptr;
    }

    std::string Localization::getString(const std::string& str) const
    {
        if (currentLanguage)
            return currentLanguage->getString(str);
        else
            return str;
    }
}
