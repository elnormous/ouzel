// Ouzel by Elviss Strazdins

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
        using std::logic_error::logic_error;
    };

    class Project final
    {
    public:
        Project(const storage::Path& initPath):
            path{initPath}
        {
            const auto directoryPath = path.getDirectory();

            std::ifstream f{path, std::ios::binary};
            std::vector<char> data{std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{}};

            const auto j = json::parse(data);
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
                            throw ProjectError{"Name of the target must be unique"};

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
                Asset::Options options;
                options.mipmaps = assetObject.hasMember("mipmaps") ? assetObject["mipmaps"].as<bool>() : false;

                assets.emplace_back(assetPath, assetName, assetType, options);
            }
        }

        const auto& getPath() const noexcept { return path; }
        const auto& getName() const noexcept { return name; }
        const auto& getIdentifier() const noexcept { return identifier; }
        const auto& getOrganization() const noexcept { return organization; }
        const auto& getOuzelPath() const noexcept { return ouzelPath; }
        const auto& getSourcePath() const noexcept { return sourcePath; }
        const auto& getTargets() const noexcept { return targets; }
        const auto& getSourceFiles() const noexcept { return sourceFiles; }
        const auto& getAssetsPath() const noexcept { return assetsPath; }
        const auto& getAssets() const noexcept { return assets; }

        void exportAssets(const std::string& targetName) const
        {
            for (const auto& asset : assets)
            {
                const auto assetPath = assetsPath / asset.path;

                auto resourceName = asset.path;
                resourceName.replaceExtension("otexture");
                const auto resourcePath = assetPath / resourceName;

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
