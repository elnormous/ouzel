// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    namespace ini
    {
        class Data;

        class Section
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
            bool deleteValue(const std::string& key);

        protected:
            void encode(std::vector<uint8_t>& data) const;

            std::string name;
            std::map<std::string, std::string> values;
        };

        class Data
        {
        public:
            Data();
            Data(const std::string& filename);
            Data(const std::vector<uint8_t>& data);

            void init(const std::string& filename);
            void init(const std::vector<uint8_t>& data);
            void save(const std::string& filename) const;
            void encode(std::vector<uint8_t>& data) const;

            inline const std::map<std::string, Section>& getSections() const { return sections; }
            bool hasSection(const std::string& name) const;
            Section& getSection(const std::string& name);
            Section getSection(const std::string& name) const;
            void setSection(const std::string& name, const Section& section);
            bool deleteSection(const std::string& name);

            bool hasBOM() const { return bom; }
            void setBOM(bool newBOM) { bom = newBOM; }

        private:
            bool bom = false;
            std::map<std::string, Section> sections;
        };
    } // namespace ini
} // namespace ouzel
