// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_ARCHIVE_HPP
#define OUZEL_STORAGE_ARCHIVE_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "storage/File.hpp"

namespace ouzel
{
    namespace storage
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

            struct Entry final
            {
                uint32_t offset;
                uint32_t size;
            };

            std::map<std::string, Entry> entries;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_ARCHIVE_HPP
