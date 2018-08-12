// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "files/File.hpp"

namespace ouzel
{
    class FileSystem;

    class Archive final
    {
    public:
        explicit Archive(FileSystem& initFileSystem): fileSystem(initFileSystem) {}
        Archive(FileSystem& initFileSystem, const std::string& filename);
        ~Archive();

        Archive(const Archive&) = delete;
        Archive& operator=(const Archive&) = delete;

        Archive(Archive&& other) = delete;
        Archive& operator=(Archive&& other) = delete;

        std::vector<uint8_t> readFile(const std::string& filename) const;

        bool fileExists(const std::string& filename) const;

    private:
        FileSystem& fileSystem;
        File file;

        struct Entry
        {
            uint32_t offset;
            uint32_t size;
        };

        std::map<std::string, Entry> entries;
    };
}
