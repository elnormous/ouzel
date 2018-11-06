// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_LANGUAGE_HPP
#define OUZEL_LANGUAGE_HPP

#include <cstdint>
#include <map>
#include <vector>

namespace ouzel
{
    class Language final
    {
    public:
        Language() {}
        explicit Language(const std::vector<uint8_t>& data);

        std::string getString(const std::string& str);

    private:
        std::map<std::string, std::string> strings;
    };
}

#endif // OUZEL_LANGUAGE_HPP
