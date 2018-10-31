// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <stdexcept>

namespace ouzel
{
    class ConfigError final: public std::runtime_error
    {
    public:
        explicit ConfigError(const std::string& str): std::runtime_error(str) {}
        explicit ConfigError(const char* str): std::runtime_error(str) {}
    };

    class DataError final: public std::runtime_error
    {
    public:
        explicit DataError(const std::string& str): std::runtime_error(str) {}
        explicit DataError(const char* str): std::runtime_error(str) {}
    };

    class FileError final: public std::runtime_error
    {
    public:
        explicit FileError(const std::string& str): std::runtime_error(str) {}
        explicit FileError(const char* str): std::runtime_error(str) {}
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

    class ThreadError final: public std::runtime_error
    {
    public:
        explicit ThreadError(const std::string& str): std::runtime_error(str) {}
        explicit ThreadError(const char* str): std::runtime_error(str) {}
    };
}
