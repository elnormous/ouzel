// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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
        friend FileSystem;
    public:
        Archive() {}
        explicit Archive(const std::string& filename);
        ~Archive();

        Archive(const Archive&) = delete;
        Archive& operator=(const Archive&) = delete;

        Archive(Archive&& other);
        Archive& operator=(Archive&& other);

        std::vector<uint8_t> readFile(const std::string& filename) const;

        bool fileExists(const std::string& filename) const;

    private:
        FileSystem* fileSystem = nullptr;
        File file;

        struct Entry
        {
            uint32_t offset;
            uint32_t size;
        };

        std::map<std::string, Entry> entries;
    };
}
