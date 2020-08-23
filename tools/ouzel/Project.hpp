// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_OUZELPROJECT_HPP
#define OUZEL_OUZELPROJECT_HPP

#include <fstream>
#include "Asset.hpp"
#include "Target.hpp"
#include "storage/FileSystem.hpp"
#include "formats/Json.hpp"

namespace ouzel
{
    class ProjectError final: public std::logic_error
    {
    public:
        explicit ProjectError(const std::string& str): std::logic_error(str) {}
        explicit ProjectError(const char* str): std::logic_error(str) {}
    };

    class Project final
    {
    public:
        Project(const storage::Path& initPath):
            path{initPath}
        {
            storage::Path directoryPath = path.getDirectory();

            std::ifstream f(path, std::ios::binary);
            std::vector<char> data{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            json::Value j = json::parse(data);
            name = j["name"].as<std::string>();
            identifier = j["identifier"].as<std::string>();
            organization = j["organization"].as<std::string>();

            ouzelPath = j["ouzelPath"].as<std::string>();

            if (j.hasMember("targets"))
                for (const auto& targetObject : j["targets"])
                {
                    const auto platform = stringToPlatform(targetObject["platform"].as<std::string>());
                    const auto targetName = targetObject.hasMember("name") ? targetObject["name"].as<std::string>() : name + ' ' + toString(platform);

                    for (const auto& otherTarget : targets)
                        if (otherTarget.name == targetName)
                            throw ProjectError("Name of the target must be unique");

                    targets.emplace_back(platform, targetName);
                }

            sourcePath = j["sourcePath"].as<std::string>();

            for (const auto& sourceFile : j["sourceFiles"])
                sourceFiles.push_back(sourceFile.as<std::string>());

            assetsPath = j["assetsPath"].as<std::string>();

            for (const auto& assetObject : j["assets"])
            {
                const auto assetPath(assetsPath / assetObject["path"].as<std::string>());
                const auto assetName = assetObject.hasMember("name") ?
                    assetObject["name"].as<std::string>() : std::string(assetPath.getStem());

                const auto assetType = stringToAssetType(assetObject["type"].as<std::string>());

                assets.emplace_back(assetPath,
                                    assetName,
                                    assetType,
                                    assetObject.hasMember("mipmaps") ? assetObject["mipmaps"].as<bool>() : false);
            }
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
        const std::vector<Asset>& getAssets() const noexcept { return assets; }

        void exportAssets(const std::string& targetName) const
        {
            for (const auto& asset : assets)
            {
                const storage::Path assetPath = assetsPath / asset.path;

                storage::Path resourceName = asset.path;
                resourceName.replaceExtension("otexture");
                const storage::Path resourcePath = assetPath / resourceName;

                const auto targetIterator = std::find_if(targets.begin(), targets.end(),
                                                         [targetName](const auto& target) noexcept {
                    return target.name == targetName;
                });

                if (targetIterator == targets.end())
                    throw std::runtime_error("Target not found");

                // TODO: check if input file exists
                // TODO: check if output file exists and is older than the input file
                // TODO: export input file to output file
            }
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
        std::vector<Asset> assets;
    };
}

#endif // OUZELPROJECT_HPP
