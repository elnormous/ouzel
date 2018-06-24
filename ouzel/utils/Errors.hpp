// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <stdexcept>

namespace ouzel
{
    class ConfigError: public std::runtime_error
    {
    public:
        explicit ConfigError(const std::string& str): std::runtime_error(str) {}
        explicit ConfigError(const char* str): std::runtime_error(str) {}
    };

    class DataError: public std::runtime_error
    {
    public:
        explicit DataError(const std::string& str): std::runtime_error(str) {}
        explicit DataError(const char* str): std::runtime_error(str) {}
    };

    class FileError: public std::runtime_error
    {
    public:
        explicit FileError(const std::string& str): std::runtime_error(str) {}
        explicit FileError(const char* str): std::runtime_error(str) {}
    };

    class NetworkError: public std::runtime_error
    {
    public:
        explicit NetworkError(const std::string& str): std::runtime_error(str) {}
        explicit NetworkError(const char* str): std::runtime_error(str) {}
    };

    class ParseError: public std::runtime_error
    {
    public:
        explicit ParseError(const std::string& str): std::runtime_error(str) {}
        explicit ParseError(const char* str): std::runtime_error(str) {}
    };

    class SystemError: public std::runtime_error
    {
    public:
        explicit SystemError(const std::string& str): std::runtime_error(str) {}
        explicit SystemError(const char* str): std::runtime_error(str) {}
    };

    class ThreadError: public std::runtime_error
    {
    public:
        explicit ThreadError(const std::string& str): std::runtime_error(str) {}
        explicit ThreadError(const char* str): std::runtime_error(str) {}
    };
}
