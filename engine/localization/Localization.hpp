// Ouzel by Elviss Strazdins

#ifndef OUZEL_LOCALIZATION_LOCALIZATION_HPP
#define OUZEL_LOCALIZATION_LOCALIZATION_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel::l10n
{
    class Language final
    {
    public:
        Language() = default;
        explicit Language(const std::vector<std::byte>& data);

        std::string getString(const std::string& str) const;

    private:
        std::map<std::string, std::string> strings;
    };

    class Localization final
    {
    public:
        void addLanguage(const std::string& name,
                         const std::vector<std::byte>& data);
        void removeLanguage(const std::string& name);
        void setLanguage(const std::string& name);
        std::string getString(const std::string& str) const;

    private:
        std::map<std::string, Language> languages;
        std::map<std::string, Language>::const_iterator currentLanguage = languages.end();
    };
}

#endif // OUZEL_LOCALIZATION_LOCALIZATION_HPP
