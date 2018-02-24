// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "files/File.hpp"

namespace ouzel
{
    class FileSystem;

    class Archive
    {
        friend FileSystem;
    public:
        Archive() {}
        ~Archive();

        Archive(const Archive&) = delete;
        Archive& operator=(const Archive&) = delete;

        Archive(Archive&&) = delete;
        Archive& operator=(Archive&&) = delete;

        bool readFile(const std::string& filename, std::vector<uint8_t>& data) const;
        bool writeFile(const std::string& filename, const std::vector<uint8_t>& data) const;

        bool fileExists(const std::string& filename) const;

    private:
        FileSystem* fileSystem = nullptr;
        File file;
    };
}
