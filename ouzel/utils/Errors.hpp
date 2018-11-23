// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_ERRORS_HPP
#define OUZEL_UTILS_ERRORS_HPP

#include <stdexcept>

namespace ouzel
{
    class DataError final: public std::runtime_error
    {
    public:
        explicit DataError(const std::string& str): std::runtime_error(str) {}
        explicit DataError(const char* str): std::runtime_error(str) {}
    };

    class ParseError final: public std::runtime_error
    {
    public:
        explicit ParseError(const std::string& str): std::runtime_error(str) {}
        explicit ParseError(const char* str): std::runtime_error(str) {}
    };

    class SystemError final: public std::runtime_error
    {
    public:
        explicit SystemError(const std::string& str): std::runtime_error(str) {}
        explicit SystemError(const char* str): std::runtime_error(str) {}
    };
}

#endif // OUZEL_UTILS_ERRORS_HPP
