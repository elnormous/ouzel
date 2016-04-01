// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

namespace ouzel
{
    class Localization
    {
    public:
        virtual ~Localization() {}

        void setLanguage(const std::string& language);
        std::string getString(const std::string& str);

    private:
        std::string _language;
    };
}
