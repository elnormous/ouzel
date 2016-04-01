// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Localization.h"
#include "Language.h"

namespace ouzel
{
    void Localization::setLanguage(const std::string& language)
    {
        auto i = _languages.find(language);

        if (i != _languages.end())
        {
            _language = i->second;
        }
        else
        {
            _language.reset();
        }
    }

    std::string Localization::getString(const std::string& str)
    {
        if (_language)
        {
            return _language->getString(str);
        }
        else
        {
            return str;
        }
    }
}
