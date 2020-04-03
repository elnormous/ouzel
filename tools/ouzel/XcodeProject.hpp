// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODEPROJECT_HPP
#define OUZEL_XCODEPROJECT_HPP

#include <array>
#include <chrono>
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
        std::random_device randomDevice;
        std::mt19937 randomEngine(randomDevice());
        std::uniform_int_distribution<uint32_t> distribution;
        uint32_t sequence = 0;

        using Id = std::array<uint8_t, 12>;

        Id generateId()
        {
            Id result;

            ++sequence;

            result[0] = 0x70; // to avoid collisions with Ouzel targets
            result[1] = static_cast<uint8_t>((sequence >> 16) & 0xFF);
            result[2] = static_cast<uint8_t>((sequence >> 8) & 0xFF);
            result[3] = static_cast<uint8_t>((sequence >> 0) & 0xFF);

            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

            result[4] = static_cast<uint8_t>((timestamp >> 24) & 0xFF);
            result[5] = static_cast<uint8_t>((timestamp >> 16) & 0xFF);
            result[6] = static_cast<uint8_t>((timestamp >> 8) & 0xFF);
            result[7] = static_cast<uint8_t>((timestamp >> 0) & 0xFF);

            uint32_t r = distribution(randomEngine);
            result[8] = static_cast<uint8_t>((r >> 24) & 0xFF);
            result[9] = static_cast<uint8_t>((r >> 16) & 0xFF);
            result[10] = static_cast<uint8_t>((r >> 8) & 0xFF);
            result[11] = static_cast<uint8_t>((r >> 0) & 0xFF);

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
            Object(const std::string& i): isa{i} {}
            virtual ~Object() = default;

            const Id& getId() const noexcept { return id; }
            const std::string& getIsa() const noexcept { return isa; }

        private:
            Id id = generateId();
            std::string isa;
        };

        class PbxFileElement: public Object
        {
        public:
            PbxFileElement(const std::string& i):
                Object{i} {}
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

        std::string toString(PbxSourceTree sourceTree)
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
            PbxFileReference(const std::string n,
                             const storage::Path& p,
                             const std::string& type,
                             PbxSourceTree tree):
                PbxFileElement{"PBXFileReference"},
                name{n},
                path{p},
                fileType{type},
                sourceTree{tree} {}

            const storage::Path& getPath() const noexcept { return path; }

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"explicitFileType", fileType},
                    {"includeInIndex", 0},
                    {"path", std::string(path)},
                    {"sourceTree", toString(sourceTree)}
                };

                if (!name.empty())
                    result["name"] = name;

                return result;
            }

        private:
            std::string name;
            storage::Path path;
            std::string fileType;
            PbxSourceTree sourceTree;
        };

        class PbxContainerItemProxy final: public Object
        {
        public:
            PbxContainerItemProxy(const Id& containerPortal,
                                  const Id& remote,
                                  const std::string& info):
                Object{"PBXContainerItemProxy"},
                containerPortalId{containerPortal},
                remoteGlobalId{remote},
                remoteInfo{info} {}

            plist::Value getValue() const
            {
                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"containerPortal", toString(containerPortalId)},
                    {"proxyType", 2},
                    {"remoteGlobalIDString", toString(remoteGlobalId)},
                    {"remoteInfo", remoteInfo}
                };
            }

        private:
            Id containerPortalId;
            Id remoteGlobalId;
            std::string remoteInfo;
        };

        class PbxReferenceProxy final: public Object
        {
        public:
            PbxReferenceProxy(const std::string& type,
                              const storage::Path& p,
                              const Id& remoteRef,
                              PbxSourceTree tree):
                Object{"PBXReferenceProxy"},
                fileType{type},
                path{p},
                remoteRefId{remoteRef},
                sourceTree{tree} {}

            plist::Value getValue() const
            {
                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"fileType", fileType},
                    {"path", std::string{path}},
                    {"remoteRef", toString(remoteRefId)},
                    {"sourceTree", toString(sourceTree)}
                };
            }

        private:
            std::string fileType;
            storage::Path path;
            const Id& remoteRefId;
            PbxSourceTree sourceTree;
        };

        class PbxBuildFile final: public Object
        {
        public:
            PbxBuildFile(const Id& fileRef):
                Object{"PBXBuildFile"}, fileRefId{fileRef} {}

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
                PbxFileElement{"PBXGroup"},
                name{n},
                path{p},
                childIds{children},
                sourceTree{tree} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"children", plist::Value::Array{}},
                    {"sourceTree", toString(sourceTree)}
                };

                if (!std::string(path).empty())
                    result["path"] = std::string(path);
                else if (!name.empty())
                    result["name"] = name;

                for (auto childId : childIds)
                    result["children"].pushBack(toString(childId));

                return result;
            }

        private:
            std::string name;
            storage::Path path;
            std::vector<Id> childIds;
            PbxSourceTree sourceTree;
        };

        class XcBuildConfiguration final: public Object
        {
        public:
            XcBuildConfiguration(const std::string& n,
                                 const std::map<std::string, std::string>& settings):
                Object{"XCBuildConfiguration"},
                name{n},
                buildSettings{settings} {}

            const std::string& getName() const noexcept { return name; }

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildSettings", plist::Value::Dictionary{}},
                    {"name", name}
                };

                for (const auto& buildSetting : buildSettings)
                    result["buildSettings"][buildSetting.first] = buildSetting.second;

                return result;
            }

        private:
            std::string name;
            std::map<std::string, std::string> buildSettings;
        };

        class XcConfigurationList final: public Object
        {
        public:
            XcConfigurationList(const std::vector<Id>& configurations,
                                const std::string& defaultConfiguration):
                Object{"XCConfigurationList"},
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
            PbxBuildPhase(const std::string& i):
                Object{i} {}
        };

        class PbxSourcesBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxSourcesBuildPhase(const std::vector<Id>& files):
                PbxBuildPhase{"PBXSourcesBuildPhase"},
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

        class PbxFrameworksBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxFrameworksBuildPhase(const std::vector<Id>& files):
                PbxBuildPhase{"PBXFrameworksBuildPhase"},
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
            PbxTarget(const std::string& i):
                Object{i} {}
        };

        class PbxNativeTarget final: public PbxTarget
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const Id& buildConfigurationList,
                            const std::vector<Id>& buildPhases,
                            const Id& productReference):
                PbxTarget{"PBXNativeTarget"},
                name{n},
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
                    {"name", name},
                    {"productName", name},
                    {"productReference", toString(productReferenceId)},
                    {"productType", "com.apple.product-type.application"}
                };

                for (auto buildPhaseId : buildPhaseIds)
                    result["buildPhases"].pushBack(toString(buildPhaseId));

                return result;
            }

        private:
            std::string name;
            Id buildConfigurationListId;
            std::vector<Id> buildPhaseIds;
            Id productReferenceId;
        };

        class PbxProject final: public Object
        {
        public:
            PbxProject(const std::string& org,
                       const Id& buildConfigurationList,
                       const Id& mainGroup,
                       const Id& productRefGroup,
                       const std::map<std::string, Id>& references,
                       const std::vector<Id>& targets):
                Object{"PBXProject"},
                organization{org},
                buildConfigurationListId{buildConfigurationList},
                mainGroupId(mainGroup),
                productRefGroupId(productRefGroup),
                projectReferences{references},
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

                if (!projectReferences.empty())
                {
                    auto references = plist::Value::Dictionary{};

                    for (const auto& projectReference : projectReferences)
                        references[projectReference.first] = toString(projectReference.second);

                    result["projectReferences"] = plist::Value::Array{references};
                }

                for (auto targetId : targetIds)
                    result["targets"].pushBack(toString(targetId));

                return result;
            }

        private:
            std::string organization;
            Id buildConfigurationListId;
            Id mainGroupId;
            Id productRefGroupId;
            std::map<std::string, Id> projectReferences;
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

            constexpr auto libouzelIosId = Id{0x30, 0x3B, 0x75, 0x33, 0x1C, 0x2A, 0x3C, 0x58, 0x00, 0xFE, 0xDE, 0x92};
            constexpr auto libouzelMacOsId = Id{0x30, 0xA3, 0x96, 0x29, 0x24, 0x37, 0x73, 0xB5, 0x00, 0xD8, 0xE2, 0x8E};
            constexpr auto libouzelTvosId = Id{0x30, 0xA3, 0x96, 0x29, 0x24, 0x37, 0x73, 0xB5, 0x00, 0xD8, 0xE2, 0x8E};
            constexpr auto ouzelId = Id{0x30, 0xA3, 0x96, 0x29, 0x24, 0x37, 0x73, 0xB5, 0x00, 0xD8, 0xE2, 0x8E};

            Output output;

            auto ouzelProjectPath = project.getOuzelPath() / "build" / "ouzel.xcodeproj";
            PbxFileReference ouzelProjectFileRef{"ouzel.xcodeproj", ouzelProjectPath,
                "wrapper.pb-project", PbxSourceTree::Group};
            output.addObject(ouzelProjectFileRef);
            PbxContainerItemProxy libouzelIosProxy{ouzelProjectFileRef.getId(),
                libouzelIosId, "libouzel_ios"};
            output.addObject(libouzelIosProxy);

            PbxReferenceProxy libouzelIosReferenceProxy{"archive.ar",
                "libouzel_ios.a", libouzelIosProxy.getId(),
                PbxSourceTree::BuildProductsDir
            };
            output.addObject(libouzelIosReferenceProxy);

            PbxContainerItemProxy libouzelMacOsProxy{ouzelProjectFileRef.getId(),
                libouzelMacOsId, "libouzel_macos"};
            output.addObject(libouzelMacOsProxy);

            PbxReferenceProxy libouzelMacOsReferenceProxy{"archive.ar",
                "libouzel_macos.a", libouzelMacOsProxy.getId(),
                PbxSourceTree::BuildProductsDir
            };
            output.addObject(libouzelMacOsReferenceProxy);

            PbxBuildFile libouzelMacOsBuildFile{libouzelMacOsReferenceProxy.getId()};
            output.addObject(libouzelMacOsBuildFile);

            PbxContainerItemProxy libouzelTvosProxy{ouzelProjectFileRef.getId(),
                libouzelTvosId, "libouzel_tvos"};
            output.addObject(libouzelTvosProxy);

            PbxReferenceProxy libouzelTvosReferenceProxy{"archive.ar",
                "libouzel_tvos.a", libouzelTvosProxy.getId(),
                PbxSourceTree::BuildProductsDir
            };
            output.addObject(libouzelTvosReferenceProxy);

            PbxContainerItemProxy ouzelProxy{ouzelProjectFileRef.getId(),
                ouzelId, "ouzel"};
            output.addObject(ouzelProxy);

            PbxReferenceProxy ouzelReferenceProxy{"compiled.mach-o.executable",
                "ouzel", ouzelProxy.getId(),
                PbxSourceTree::BuildProductsDir
            };
            output.addObject(ouzelReferenceProxy);

            PbxGroup frameworksGroup{"Frameworks", storage::Path{},
                {}, PbxSourceTree::Group};
            output.addObject(frameworksGroup);

            PbxGroup ouzelPoductRefGroup{"Products", storage::Path{},
                {libouzelIosReferenceProxy.getId(), libouzelMacOsReferenceProxy.getId(), libouzelTvosReferenceProxy.getId(), ouzelReferenceProxy.getId()},
                PbxSourceTree::Group};
            output.addObject(ouzelPoductRefGroup);

            std::vector<Id> productFileIds;

            PbxFileReference productFile{"", storage::Path{project.getName() + ".app",},
                "wrapper.application",
                PbxSourceTree::BuildProductsDir};
            output.addObject(productFile);

            PbxGroup productRefGroup{"Products", storage::Path{},
                {productFile.getId()}, PbxSourceTree::Group};
            output.addObject(productRefGroup);

            std::vector<Id> buildFileIds;
            std::vector<Id> sourceFileIds;

            for (const auto& sourceFile : project.getSourceFiles())
            {
                const auto extension = sourceFile.getExtension();
                const std::string fileType = extension == "plist" ?
                    "text.plist.xml" :
                    "sourcecode.cpp." + extension;

                PbxFileReference fileReference{"", sourceFile, fileType, PbxSourceTree::Group};
                output.addObject(fileReference);
                sourceFileIds.push_back(fileReference.getId());

                PbxBuildFile buildFile{fileReference.getId()};
                output.addObject(buildFile);
                buildFileIds.push_back(buildFile.getId());
            }

            PbxGroup sourceGroup{"src", storage::Path{"src"},
                sourceFileIds, PbxSourceTree::Group};
            output.addObject(sourceGroup);

            PbxGroup mainGroup{"", storage::Path{},
                {frameworksGroup.getId(), ouzelProjectFileRef.getId(), productRefGroup.getId(), sourceGroup.getId()},
                PbxSourceTree::Group};
            output.addObject(mainGroup);

            const auto headerSearchPath = std::string(project.getOuzelPath() / "engine");
            XcBuildConfiguration debugConfiguration{"Debug", {
                {"HEADER_SEARCH_PATHS", headerSearchPath},
                {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"}
            }};
            output.addObject(debugConfiguration);

            XcBuildConfiguration releaseConfiguration{"Release", {
                {"HEADER_SEARCH_PATHS", headerSearchPath},
                {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"}
            }};
            output.addObject(releaseConfiguration);

            XcConfigurationList projectConfigurationList{
                {debugConfiguration.getId(), releaseConfiguration.getId()},
                releaseConfiguration.getName()};
            output.addObject(projectConfigurationList);

            std::vector<Id> targetIds;

            for (const auto platform : project.getPlatforms())
            {
                // TODO: do it for all platforms
                if (platform == Platform::MacOs)
                {
                    XcBuildConfiguration targetDebugConfiguration{"Debug", {
                        {"PRODUCT_NAME", project.getName()}
                    }};
                    output.addObject(targetDebugConfiguration);

                    XcBuildConfiguration targetReleaseConfiguration{"Release", {
                        {"PRODUCT_NAME", project.getName()}
                    }};
                    output.addObject(targetReleaseConfiguration);

                    XcConfigurationList targetConfigurationList{
                        {targetDebugConfiguration.getId(), targetReleaseConfiguration.getId()},
                        targetReleaseConfiguration.getName()};
                    output.addObject(targetConfigurationList);

                    PbxSourcesBuildPhase sourcesBuildPhase{buildFileIds};
                    output.addObject(sourcesBuildPhase);

                    PbxFrameworksBuildPhase frameworksBuildPhase{{libouzelMacOsBuildFile.getId()}};
                    output.addObject(frameworksBuildPhase);

                    PbxNativeTarget nativeTarget{project.getName() + " macOS",
                        targetConfigurationList.getId(),
                        {sourcesBuildPhase.getId(), frameworksBuildPhase.getId()},
                        productFile.getId()};
                    output.addObject(nativeTarget);
                    targetIds.push_back(nativeTarget.getId());
                }
            }

            PbxProject pbxProject{project.getOrganization(),
                projectConfigurationList.getId(),
                mainGroup.getId(),
                productRefGroup.getId(),
                {{"ProductGroup", ouzelPoductRefGroup.getId()},
                 {"ProjectRef", ouzelProjectFileRef.getId()}},
                targetIds};
            output.addRootObject(pbxProject);

            // TODO: resource build phases (PBXResourcesBuildPhase section)

            std::ofstream file(projectFile, std::ios::trunc);
            file << output.encode();
        }

    private:
        class Output final
        {
        public:
            template <class T>
            void addObject(const T& object)
            {
                root["objects"][toString(object.getId())] = object.getValue();
            }

            template <class T>
            void addRootObject(const T& object)
            {
                root["objects"][toString(object.getId())] = object.getValue();
                root["rootObject"] = toString(object.getId());
            }

            std::string encode() const { return plist::encode(root); }

        private:
            plist::Value root = plist::Value::Dictionary{
                {"archiveVersion", 1},
                {"classes", plist::Value::Dictionary{}},
                {"objectVersion", 50},
                {"objects", plist::Value::Dictionary{}}
            };
        };

        const OuzelProject& project;
    };
}

#endif // OUZEL_XCODEPROJECT_HPP
