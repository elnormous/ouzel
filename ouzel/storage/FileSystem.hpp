// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_FILESYSTEM_HPP
#define OUZEL_STORAGE_FILESYSTEM_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace ouzel
{
    class Engine;

    namespace storage
    {
        class Archive;

        class FileSystem final
        {
        public:
            static const std::string DIRECTORY_SEPARATOR;

            explicit FileSystem(Engine& initEngine);
            FileSystem(const FileSystem&) = delete;
            FileSystem& operator=(const FileSystem&) = delete;

            FileSystem(FileSystem&&) = delete;
            FileSystem& operator=(FileSystem&&) = delete;

            std::string getStorageDirectory(bool user = true) const;
            std::string getTempDirectory() const;

            std::vector<uint8_t> readFile(const std::string& filename, bool searchResources = true) const;
            void writeFile(const std::string& filename, const std::vector<uint8_t>& data) const;

            bool resourceFileExists(const std::string& filename) const;
            std::string getPath(const std::string& filename, bool searchResources = true) const;
            void addResourcePath(const std::string& path);
            void addArchive(Archive* archive);
            void removeArchive(Archive* archive);

            static std::string getExtensionPart(const std::string& path);
            static std::string getFilenamePart(const std::string& path);
            static std::string getDirectoryPart(const std::string& path);
            static bool pathIsRelative(const std::string& path);

            bool directoryExists(const std::string& dirname) const;
            bool fileExists(const std::string& filename) const;

        private:
            Engine& engine;
            std::string appPath;
            std::vector<std::string> resourcePaths;
            std::vector<Archive*> archives;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_FILESYSTEM_HPP
