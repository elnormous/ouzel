// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODEPROJECT_HPP
#define OUZEL_XCODEPROJECT_HPP

#include <array>
#include <fstream>
#include <map>
#include <random>
#include <stack>
#include <type_traits>
#include "OuzelProject.hpp"
#include "storage/FileSystem.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace
    {
        using Id = std::array<uint8_t, 12>;

        Id generateId()
        {
            std::random_device randomDevice;
            std::mt19937 randomEngine(randomDevice());
            std::uniform_int_distribution<uint32_t> distribution(0, 255);

            Id result;

            for (auto& i : result)
                i = static_cast<uint8_t>(distribution(randomEngine));

            return result;
        }

        std::string toString(const Id& i)
        {
            std::string result;
            result.reserve(2 * i.size());
            for (const auto b : i)
            {
                constexpr char digits[] = "0123456789ABCDEF";
                result.push_back(digits[(b >> 4) & 0x0F]);
                result.push_back(digits[(b >> 0) & 0x0F]);
            }
            return result;
        }

        class Object
        {
        public:
            Object(const std::string& i,
                   const std::string& n): isa{i}, name{n} {}
            virtual ~Object() = default;

            const Id& getId() const noexcept { return id; }
            const std::string& getIsa() const noexcept { return isa; }
            const std::string& getName() const noexcept { return name; }

        private:
            Id id = generateId();
            std::string isa;
            std::string name;
        };

        class PbxFileElement: public Object
        {
        public:
            PbxFileElement(const std::string& i,
                           const std::string& n):
                Object{i, n} {}
        };

        enum class PbxSourceTree
        {
            Absolute,
            Group,
            SourceRoot,
            BuildProductsDir,
            SdkRoot,
            DeveloperDir
        };

        std::string sourceTreeToString(PbxSourceTree sourceTree)
        {
            switch (sourceTree)
            {
                case PbxSourceTree::Absolute: return "<absolute>";
                case PbxSourceTree::Group: return "<group>";
                case PbxSourceTree::SourceRoot: return "SOURCE_ROOT";
                case PbxSourceTree::BuildProductsDir: return "BUILT_PRODUCTS_DIR";
                case PbxSourceTree::SdkRoot: return "SDKROOT";
                case PbxSourceTree::DeveloperDir: return "DEVELOPER_DIR";
            }
        }

        class PbxFileReference final: public PbxFileElement
        {
        public:
            PbxFileReference(const storage::Path& p, PbxSourceTree tree):
                PbxFileElement{"PBXFileReference", std::string{p}}, path{p}, sourceTree{tree} {}
            const storage::Path& getPath() const noexcept { return path; }

            plist::Value getValue() const
            {
                const auto extension = path.getExtension();
                const std::string fileType = (sourceTree == PbxSourceTree::BuildProductsDir) ?
                    "wrapper.application" :
                    extension == "plist" ? "text.plist.xml" : "sourcecode.cpp." + extension;

                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"explicitFileType", fileType},
                    {"includeInIndex", 0},
                    {"path", std::string(path)},
                    {"sourceTree", sourceTreeToString(sourceTree)}
                };
            }

        private:
            storage::Path path;
            PbxSourceTree sourceTree;
        };

        class PbxBuildFile final: public Object
        {
        public:
            PbxBuildFile(const std::string& n,
                         const Id& fileRef):
                Object{"PBXBuildFile", n}, fileRefId{fileRef} {}

            plist::Value getValue() const
            {
                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"fileRef", toString(fileRefId)}
                };
            }

        private:
            Id fileRefId;
        };

        class PbxGroup final: public PbxFileElement
        {
        public:
            PbxGroup(const std::string& n,
                     const storage::Path& p,
                     const std::vector<Id>& children,
                     PbxSourceTree tree):
                PbxFileElement{"PBXGroup", n},
                path{p},
                childIds{children},
                sourceTree{tree} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"children", plist::Value::Array{}},
                    {"sourceTree", sourceTreeToString(sourceTree)}
                };

                if (!std::string(path).empty())
                    result["path"] = std::string(path);
                else if (!getName().empty())
                    result["name"] = getName();

                for (auto childId : childIds)
                    result["children"].pushBack(toString(childId));

                return result;
            }

        private:
            storage::Path path;
            std::vector<Id> childIds;
            PbxSourceTree sourceTree;
        };

        class XcBuildConfiguration final: public Object
        {
        public:
            XcBuildConfiguration(const std::string& n,
                                 const std::string& p = "",
                                 const std::string& h = ""):
                Object{"XCBuildConfiguration", n},
                productName{p},
                headerSearchPath{h} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildSettings", plist::Value::Dictionary{}},
                    {"name", getName()}
                };

                if (!productName.empty())
                    result["buildSettings"]["PRODUCT_NAME"] = productName;

                if (!headerSearchPath.empty())
                    result["buildSettings"]["HEADER_SEARCH_PATHS"] = headerSearchPath;

                return result;
            }

        private:
            std::string productName;
            std::string headerSearchPath;
        };

        class XcConfigurationList final: public Object
        {
        public:
            XcConfigurationList(const std::string& n,
                                const std::vector<Id>& configurations,
                                const std::string& defaultConfiguration):
                Object{"XCConfigurationList", n},
                configurationIds{configurations},
                defaultConfigurationName{defaultConfiguration} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildConfigurations", plist::Value::Array{}},
                    {"defaultConfigurationIsVisible", 0},
                    {"defaultConfigurationName", defaultConfigurationName},
                };

                for (auto configurationId : configurationIds)
                    result["buildConfigurations"].pushBack(toString(configurationId));

                return result;
            }

        private:
            std::vector<Id> configurationIds;
            std::string defaultConfigurationName;
        };

        class PbxBuildPhase: public Object
        {
        public:
            PbxBuildPhase(const std::string& i,
                          const std::string& n):
                Object{i, n} {}
        };

        class PbxSourcesBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxSourcesBuildPhase(const std::string& n,
                                 const std::vector<Id>& files):
                PbxBuildPhase{"PBXSourcesBuildPhase", n},
                fileIds{files} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildActionMask", 2147483647},
                    {"files", plist::Value::Array{}},
                    {"runOnlyForDeploymentPostprocessing", 0},
                };

                for (auto fileId : fileIds)
                    result["files"].pushBack(toString(fileId));

                return result;
            }

        private:
            std::vector<Id> fileIds;
        };

        class PbxTarget: public Object
        {
        public:
            PbxTarget(const std::string& i,
                      const std::string& n):
                Object{i, n} {}
        };

        class PbxNativeTarget final: public PbxTarget
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const Id& buildConfigurationList,
                            const std::vector<Id>& buildPhases,
                            const Id& productReference):
                PbxTarget{"PBXNativeTarget", n},
                buildConfigurationListId{buildConfigurationList},
                buildPhaseIds{buildPhases},
                productReferenceId{productReference} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildConfigurationList", toString(buildConfigurationListId)},
                    {"buildPhases", plist::Value::Array{}},
                    {"buildRules", plist::Value::Array{}},
                    {"dependencies", plist::Value::Array{}},
                    {"name", getName()},
                    {"productName", getName()},
                    {"productReference", toString(productReferenceId)},
                    {"productType", "com.apple.product-type.application"}
                };

                for (auto buildPhaseId : buildPhaseIds)
                    result["buildPhases"].pushBack(toString(buildPhaseId));

                return result;
            }

        private:
            Id buildConfigurationListId;
            std::vector<Id> buildPhaseIds;
            Id productReferenceId;
        };

        class PbxProject final: public Object
        {
        public:
            PbxProject(const std::string& n,
                       const std::string& org,
                       const Id& buildConfigurationList,
                       const Id& mainGroup,
                       const Id& productRefGroup,
                       const std::vector<Id>& targets):
                Object{"PBXProject", n},
                organization{org},
                buildConfigurationListId{buildConfigurationList},
                mainGroupId(mainGroup),
                productRefGroupId(productRefGroup),
                targetIds{targets} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"attributes", plist::Value::Dictionary{
                        {"LastUpgradeCheck", "0800"},
                        {"ORGANIZATIONNAME", organization}
                    }},
                    {"buildConfigurationList", toString(buildConfigurationListId)},
                    {"compatibilityVersion", "Xcode 9.3"},
                    {"developmentRegion", "en"},
                    {"hasScannedForEncodings", 0},
                    {"knownRegions", plist::Value::Array{"en", "Base"}},
                    {"mainGroup", toString(mainGroupId)},
                    {"productRefGroup", toString(productRefGroupId)},
                    {"projectDirPath", ""},
                    {"projectRoot", ""},
                    {"targets", plist::Value::Array{}}
                };

                for (auto targetId : targetIds)
                    result["targets"].pushBack(toString(targetId));

                return result;
            }

        private:
            std::string organization;
            Id buildConfigurationListId;
            Id mainGroupId;
            Id productRefGroupId;
            std::vector<Id> targetIds;
        };
    }

    class XcodeProject final
    {
    public:
        XcodeProject(const OuzelProject& p):
            project(p)
        {
        }

        void generate()
        {
            auto projectDirectory = project.getDirectoryPath() / storage::Path{project.getName() + ".xcodeproj"};
            auto projectDirectoryType = projectDirectory.getType();

            if (projectDirectoryType == storage::Path::Type::NotFound)
                storage::FileSystem::createDirectory(projectDirectory);
            else if (projectDirectoryType != storage::Path::Type::Directory)
            {
                storage::FileSystem::deleteFile(projectDirectory);
                storage::FileSystem::createDirectory(projectDirectory);
            }

            auto projectFile = projectDirectory / storage::Path{"project.pbxproj"};

            std::vector<Id> sourceFileIds;
            std::vector<Id> productFileIds;
            std::vector<PbxFileReference> fileReferences;

            PbxFileReference productFile{storage::Path{project.getName() + ".app"},
                PbxSourceTree::BuildProductsDir};
            productFileIds.push_back(productFile.getId());

            std::vector<Id> buildFileIds;
            std::vector<PbxBuildFile> buildFiles;

            for (const auto& sourceFile : project.getSourceFiles())
            {
                PbxFileReference fileReference{sourceFile, PbxSourceTree::Group};
                sourceFileIds.push_back(fileReference.getId());

                PbxBuildFile buildFile{fileReference.getName(), fileReference.getId()};
                buildFileIds.push_back(buildFile.getId());

                fileReferences.push_back(std::move(fileReference));
                buildFiles.push_back(std::move(buildFile));
            }

            PbxGroup productRefGroup{"Products", storage::Path{},
                productFileIds, PbxSourceTree::Group};
            PbxGroup sourceGroup{"src", storage::Path{"src"},
                sourceFileIds, PbxSourceTree::Group};
            PbxGroup mainGroup{"", storage::Path{},
                std::vector<Id>{productRefGroup.getId(), sourceGroup.getId()},
                PbxSourceTree::Group};

            std::vector<XcBuildConfiguration> configurations;
            std::vector<XcConfigurationList> configurationLists;

            XcBuildConfiguration debugConfiguration{"Debug"};
            XcBuildConfiguration releaseConfiguration{"Release"};
            XcConfigurationList projectConfigurationList{"Project",
                std::vector<Id>{debugConfiguration.getId(), releaseConfiguration.getId()},
                releaseConfiguration.getName()};

            std::vector<PbxSourcesBuildPhase> sourcesBuildPhases;

            std::vector<Id> targetIds;
            std::vector<PbxNativeTarget> nativeTargets;

            for (const auto platform : project.getPlatforms())
            {
                // TODO: do it for all platforms
                if (platform == Platform::MacOs)
                {
                    const auto headerSearchPath = std::string(project.getOuzelPath() / "engine");

                    XcBuildConfiguration targetDebugConfiguration{"Debug",
                        project.getName(), headerSearchPath};
                    XcBuildConfiguration targetReleaseConfiguration{"Release",
                        project.getName(), headerSearchPath};
                    XcConfigurationList targetConfigurationList{"Target",
                        std::vector<Id>{targetDebugConfiguration.getId(), targetReleaseConfiguration.getId()},
                        targetReleaseConfiguration.getName()};

                    PbxSourcesBuildPhase buildSourcesPhase{"Sources", buildFileIds};

                    PbxNativeTarget nativeTarget{project.getName() + " macOS",
                        targetConfigurationList.getId(),
                        std::vector<Id>{buildSourcesPhase.getId()},
                        productFile.getId()};
                    targetIds.push_back(nativeTarget.getId());
                    nativeTargets.push_back(std::move(nativeTarget));

                    configurationLists.push_back(std::move(targetConfigurationList));
                    configurations.push_back(std::move(targetDebugConfiguration));
                    configurations.push_back(std::move(targetReleaseConfiguration));
                    sourcesBuildPhases.push_back(std::move(buildSourcesPhase));
                }
            }

            PbxProject pbxProject{"Project object",
                project.getOrganization(),
                projectConfigurationList.getId(),
                mainGroup.getId(),
                productRefGroup.getId(),
                targetIds};

            configurations.push_back(std::move(debugConfiguration));
            configurations.push_back(std::move(releaseConfiguration));
            configurationLists.push_back(std::move(projectConfigurationList));

            std::vector<PbxGroup> groups{
                std::move(mainGroup),
                std::move(productRefGroup),
                std::move(sourceGroup)};
            fileReferences.push_back(std::move(productFile));

            plist::Value root = plist::Value::Dictionary{
                {"archiveVersion", 1},
                {"classes", plist::Value::Dictionary{}},
                {"objectVersion", 50},
                {"objects", plist::Value::Dictionary{}}
            };

            // PBXBuildFile section
            for (const auto& buildFile : buildFiles)
                root["objects"][toString(buildFile.getId())] = buildFile.getValue();

            // PBXFileReference section
            for (const auto& fileReference : fileReferences)
                root["objects"][toString(fileReference.getId())] = fileReference.getValue();

            // frameworks buid phase
            // PBXFrameworksBuildPhase section
            // TODO

            // PBXGroup section
            for (const auto& group : groups)
                root["objects"][toString(group.getId())] = group.getValue();

            // PBXNativeTarget section
            for (const auto& nativeTarget : nativeTargets)
                root["objects"][toString(nativeTarget.getId())] = nativeTarget.getValue();

            // PBXProject section
            root["objects"][toString(pbxProject.getId())] = pbxProject.getValue();

            // resource build phases
            // PBXResourcesBuildPhase section
            // TODO

            // source file build phase
            // PBXSourcesBuildPhase section
            for (const auto& sourcesBuildPhase : sourcesBuildPhases)
                root["objects"][toString(sourcesBuildPhase.getId())] = sourcesBuildPhase.getValue();

            // configurations
            // XCBuildConfiguration section
            for (const auto& configuration : configurations)
                root["objects"][toString(configuration.getId())] = configuration.getValue();

            // XCConfigurationList section
            for (const auto& configurationList : configurationLists)
                root["objects"][toString(configurationList.getId())] = configurationList.getValue();

            root["rootObject"] = toString(pbxProject.getId());

            std::ofstream file(projectFile, std::ios::trunc);
            file << plist::encode(root);
        }

    private:
        const OuzelProject& project;
    };
}

#endif // OUZEL_XCODEPROJECT_HPP
