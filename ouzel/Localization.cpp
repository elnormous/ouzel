// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Localization.h"

namespace ouzel
{
    void Localization::setLanguage(const std::string& language)
    {
        _language = language;
    }

    std::string Localization::getString(const std::string& str)
    {
        return str;
    }
}
