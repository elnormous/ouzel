// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <memory>
#include <cstdio>
#include <cstdint>

namespace ouzel
{
    class FileSystem;

    class File
    {
    public:
        enum class Mode
        {
            READ,
            WRITE,
            APPEND
        };

        File(const std::string& filename, Mode mode, bool binary);

        operator bool() const
        {
            return static_cast<bool>(file);
        }

        bool isOpen() const
        {
            return static_cast<bool>(file);
        }

        const std::shared_ptr<FILE>& getFile() const { return file; }

        int64_t read(char* buffer, uint32_t size);
        int64_t write(const char* buffer, uint32_t size);

    protected:
        std::shared_ptr<FILE> file;
    };
}
