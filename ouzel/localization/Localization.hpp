// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <map>
#include <string>

namespace ouzel
{
    class Language;

    class Localization final
    {
    public:
        void addLanguage(const std::string& name, const std::string& filename);
        void setLanguage(const std::string& language);
        std::string getString(const std::string& str);

    private:
        std::map<std::string, std::shared_ptr<Language>> languages;
        std::shared_ptr<Language> currentLanguage;
    };
}
