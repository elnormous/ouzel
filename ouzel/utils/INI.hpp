// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef INI_HPP
#define INI_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace ini
    {
        class Data;

        class Section final
        {
            friend Data;
        public:
            Section();
            Section(const std::string& initName);

            inline const std::string& getName() const { return name; }
            inline void setName(const std::string& newName) { name = newName; }

            inline const std::map<std::string, std::string>& getValues() const { return values; }
            bool hasValue(const std::string& key) const;
            const std::string& getValue(const std::string& key);
            std::string getValue(const std::string& key) const;
            const std::string& getValue(const std::string& key, const std::string& defaultValue) const;
            void setValue(const std::string& key, const std::string& value);
            void deleteValue(const std::string& key);

        private:
            void encode(std::vector<uint8_t>& data) const;

            std::string name;
            std::map<std::string, std::string> values;
        };

        class Data final
        {
        public:
            Data();
            Data(const std::vector<uint8_t>& data);

            std::vector<uint8_t> encode() const;

            inline const std::map<std::string, Section>& getSections() const { return sections; }
            bool hasSection(const std::string& name) const;
            Section& getSection(const std::string& name);
            Section getSection(const std::string& name) const;
            void setSection(const std::string& name, const Section& section);
            void deleteSection(const std::string& name);

            bool hasBOM() const { return bom; }
            void setBOM(bool newBOM) { bom = newBOM; }

        private:
            bool bom = false;
            std::map<std::string, Section> sections;
        };
    } // namespace ini
} // namespace ouzel

#endif // INI_HPP
