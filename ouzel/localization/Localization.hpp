// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_LOCALIZATION_LOCALIZATION_HPP
#define OUZEL_LOCALIZATION_LOCALIZATION_HPP

#include <cstdint>
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    class Language;

    class Localization final
    {
    public:
        void addLanguage(const std::string& name, const std::vector<uint8_t>& data);
        void setLanguage(const std::string& language);
        std::string getString(const std::string& str) const;

    private:
        std::map<std::string, std::shared_ptr<Language>> languages;
        std::shared_ptr<Language> currentLanguage;
    };
}

#endif // OUZEL_LOCALIZATION_LOCALIZATION_HPP
