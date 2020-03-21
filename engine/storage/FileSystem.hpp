// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_FILESYSTEM_HPP
#define OUZEL_STORAGE_FILESYSTEM_HPP

#include <string>
#include <vector>
#include <cstdint>
#include "storage/Archive.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    class Engine;

    namespace storage
    {
        class FileSystem final
        {
        public:
            explicit FileSystem(Engine& initEngine);

            std::string getStorageDirectory(const bool user = true) const;
            std::string getTempDirectory() const;

            std::vector<std::uint8_t> readFile(const std::string& filename, const bool searchResources = true);
            void writeFile(const std::string& filename, const std::vector<std::uint8_t>& data) const;

            bool resourceFileExists(const std::string& filename) const;

            std::string getPath(const std::string& filename, const bool searchResources = true) const
            {
                if (Path(filename).isAbsolute())
                {
                    if (fileExists(filename))
                        return filename;
                }
                else
                {
                    std::string str = appPath + Path::directorySeparator + filename;

                    if (fileExists(str))
                        return str;

                    if (searchResources)
                        for (const std::string& path : resourcePaths)
                        {
                            if (Path(path).isAbsolute()) // if resource path is absolute
                                str = path + Path::directorySeparator + filename;
                            else
                                str = appPath + Path::directorySeparator + path + Path::directorySeparator + filename;

                            if (fileExists(str))
                                return str;
                        }
                }

                throw std::runtime_error("Could not get path for " + filename);
            }

            void addResourcePath(const std::string& path)
            {
                const auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

                if (i == resourcePaths.end())
                    resourcePaths.push_back(path);
            }

            void removeResourcePath(const std::string& path)
            {
                const auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

                if (i != resourcePaths.end())
                    resourcePaths.erase(i);
            }

            void addArchive(const std::string& name, Archive&& archive)
            {
                archives.emplace_back(name, std::move(archive));
            }

            void removeArchive(const std::string& name)
            {
                for (auto i = archives.begin(); i != archives.end();)
                    if (i->first == name)
                        i = archives.erase(i);
                    else
                        ++i;
            }

            bool directoryExists(const std::string& dirname) const;
            bool fileExists(const std::string& filename) const;

        private:
            Engine& engine;
            std::string appPath;
            std::vector<std::string> resourcePaths;
            std::vector<std::pair<std::string, Archive>> archives;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_FILESYSTEM_HPP
