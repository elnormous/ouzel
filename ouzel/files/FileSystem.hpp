// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace ouzel
{
    class Engine;
    class Archive;

    class FileSystem
    {
        friend Engine;
    public:
        ~FileSystem() = default;

        FileSystem(const FileSystem&) = delete;
        FileSystem& operator=(const FileSystem&) = delete;

        FileSystem(FileSystem&&) = delete;
        FileSystem& operator=(FileSystem&&) = delete;

        static const std::string DIRECTORY_SEPARATOR;

        virtual std::string getStorageDirectory(bool user = true) const;
        virtual std::string getTempDirectory() const;

        virtual bool readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources = true) const;
        virtual bool writeFile(const std::string& filename, const std::vector<uint8_t>& data) const;

        bool resourceFileExists(const std::string& filename) const;
        std::string getPath(const std::string& filename, bool searchResources = true) const;
        void addResourcePath(const std::string& path);
        void addArchive(Archive* archive);
        void removeArchive(Archive* archive);

        static std::string getExtensionPart(const std::string& path);
        static std::string getFilenamePart(const std::string& path);
        static std::string getDirectoryPart(const std::string& path);
        virtual bool isAbsolutePath(const std::string& path) const;

        virtual bool directoryExists(const std::string& dirname) const;
        virtual bool fileExists(const std::string& filename) const;

    protected:
        FileSystem();

        std::string appPath;
        std::vector<std::string> resourcePaths;
        std::vector<Archive*> archives;
    };
}
