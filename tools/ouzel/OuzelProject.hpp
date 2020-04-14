// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_OUZELPROJECT_HPP
#define OUZEL_OUZELPROJECT_HPP

#include <fstream>
#include "Target.hpp"
#include "storage/FileSystem.hpp"
#include "utils/Json.hpp"

namespace ouzel
{
    class OuzelProject final
    {
    public:
        OuzelProject(const storage::Path& initPath):
            path{initPath}
        {
            storage::Path directoryPath = path.getDirectory();

            std::ifstream f(path, std::ios::binary);
            std::vector<char> data{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            json::Data j(data);
            name = j["name"].as<std::string>();
            identifier = j["identifier"].as<std::string>();
            organization = j["organization"].as<std::string>();

            ouzelPath = j["ouzelPath"].as<std::string>();

            if (j.hasMember("targets"))
                for (const auto& t : j["targets"])
                {
                    Target target(stringToPlatform(t["platform"].as<std::string>()),
                                  t.hasMember("name") ? t["name"].as<std::string>() : name);
                    targets.push_back(std::move(target));
                }

            sourcePath = j["sourcePath"].as<std::string>();

            for (const auto& sourceFile : j["sourceFiles"])
                sourceFiles.push_back(sourceFile.as<std::string>());

            assetsPath = j["assetsPath"].as<std::string>();
        }

        const storage::Path& getPath() const noexcept { return path; }
        const std::string& getName() const noexcept { return name; }
        const std::string& getIdentifier() const noexcept { return identifier; }
        const std::string& getOrganization() const noexcept { return organization; }
        const storage::Path& getOuzelPath() const noexcept { return ouzelPath; }
        const storage::Path& getSourcePath() const noexcept { return sourcePath; }
        const std::vector<Target>& getTargets() const noexcept { return targets; }
        const std::vector<storage::Path>& getSourceFiles() const noexcept { return sourceFiles; }
        const storage::Path& getAssetsPath() const noexcept { return assetsPath; }

        void exportAssets()
        {
            /*storage::Path resourcesPath = directoryPath / storage::Path("Resources");

            for (const auto& asset : j["assets"])
            {
                storage::Path assetPath(assetsPath / asset["path"].as<std::string>());

                storage::Path resourceName = asset["path"].as<std::string>();
                storage::Path resourcePath = resourcesPath / resourceName.getStem() + std::string(".otexture");

                // TODO: check if input file exists
                // TODO: check if output file exists and is older than the input file
                // TODO: export input file to output file
            }*/
        }

    private:
        const storage::Path path;
        std::string name;
        std::string identifier;
        std::string organization;
        storage::Path ouzelPath;
        storage::Path sourcePath;
        std::vector<Target> targets;
        std::vector<storage::Path> sourceFiles;
        storage::Path assetsPath;
    };
}

#endif // OUZELPROJECT_HPP
