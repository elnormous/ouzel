// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PROJECT_HPP
#define OUZEL_XCODE_PROJECT_HPP

#include <fstream>
#include "OuzelProject.hpp"
#include "PbxFileReference.hpp"
#include "PbxContainerItemProxy.hpp"
#include "PbxReferenceProxy.hpp"
#include "PbxBuildFile.hpp"
#include "PbxGroup.hpp"
#include "XcBuildConfiguration.hpp"
#include "XcConfigurationList.hpp"
#include "PbxSourcesBuildPhase.hpp"
#include "PbxFrameworksBuildPhase.hpp"
#include "PbxNativeTarget.hpp"
#include "PbxProject.hpp"
#include "storage/FileSystem.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class Project final
        {
        public:
            Project(const OuzelProject& p):
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

                PbxFileReference quartzCoreRef{"QuartzCore.framework",
                    storage::Path{"System/Library/Frameworks/QuartzCore.framework"},
                    "wrapper.framework", PbxSourceTree::SdkRoot
                };
                output.addObject(quartzCoreRef);

                auto ouzelProjectPath = project.getOuzelPath() / "build" / "ouzel.xcodeproj";
                PbxFileReference ouzelProjectFileRef{"ouzel.xcodeproj", ouzelProjectPath,
                    "wrapper.pb-project", PbxSourceTree::Group};
                output.addObject(ouzelProjectFileRef);
                PbxContainerItemProxy libouzelIosProxy{ouzelProjectFileRef,
                    libouzelIosId, "libouzel_ios"};
                output.addObject(libouzelIosProxy);

                PbxReferenceProxy libouzelIosReferenceProxy{"", "libouzel_ios.a",
                    "archive.ar",
                    PbxSourceTree::BuildProductsDir,
                    libouzelIosProxy
                };
                output.addObject(libouzelIosReferenceProxy);

                PbxContainerItemProxy libouzelMacOsProxy{ouzelProjectFileRef,
                    libouzelMacOsId, "libouzel_macos"};
                output.addObject(libouzelMacOsProxy);

                PbxReferenceProxy libouzelMacOsReferenceProxy{"", "libouzel_macos.a",
                    "archive.ar",
                    PbxSourceTree::BuildProductsDir,
                    libouzelMacOsProxy
                };
                output.addObject(libouzelMacOsReferenceProxy);

                PbxContainerItemProxy libouzelTvosProxy{ouzelProjectFileRef,
                    libouzelTvosId, "libouzel_tvos"};
                output.addObject(libouzelTvosProxy);

                PbxReferenceProxy libouzelTvosReferenceProxy{"", "libouzel_tvos.a",
                    "archive.ar",
                    PbxSourceTree::BuildProductsDir,
                    libouzelTvosProxy
                };
                output.addObject(libouzelTvosReferenceProxy);

                PbxContainerItemProxy ouzelProxy{ouzelProjectFileRef,
                    ouzelId, "ouzel"};
                output.addObject(ouzelProxy);

                PbxReferenceProxy ouzelReferenceProxy{"", "ouzel",
                    "compiled.mach-o.executable",
                    PbxSourceTree::BuildProductsDir,
                    ouzelProxy
                };
                output.addObject(ouzelReferenceProxy);

                PbxGroup frameworksGroup{"Frameworks", storage::Path{},
                    {quartzCoreRef.getId()}, PbxSourceTree::Group};
                output.addObject(frameworksGroup);

                PbxGroup ouzelPoductRefGroup{"Products", storage::Path{},
                    {libouzelIosReferenceProxy.getId(), libouzelMacOsReferenceProxy.getId(), libouzelTvosReferenceProxy.getId(), ouzelReferenceProxy.getId()},
                    PbxSourceTree::Group};
                output.addObject(ouzelPoductRefGroup);

                std::vector<Id> productFileIds;

                PbxFileReference productFile{"", storage::Path{project.getName() + ".app"},
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

                    PbxBuildFile buildFile{fileReference};
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
                    {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"},
                    {"GCC_OPTIMIZATION_LEVEL", "0"},
                    {"HEADER_SEARCH_PATHS", headerSearchPath}
                }};
                output.addObject(debugConfiguration);

                XcBuildConfiguration releaseConfiguration{"Release", {
                    {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"},
                    {"HEADER_SEARCH_PATHS", headerSearchPath}
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

                        PbxBuildFile quartzCoreBuildFile{quartzCoreRef};
                        output.addObject(quartzCoreBuildFile);

                        PbxBuildFile libouzelMacOsBuildFile{libouzelMacOsReferenceProxy};
                        output.addObject(libouzelMacOsBuildFile);

                        PbxFrameworksBuildPhase frameworksBuildPhase{{quartzCoreBuildFile.getId(), libouzelMacOsBuildFile.getId()}};
                        output.addObject(frameworksBuildPhase);

                        PbxNativeTarget nativeTarget{project.getName() + " macOS",
                            targetConfigurationList,
                            {sourcesBuildPhase.getId(), frameworksBuildPhase.getId()},
                            productFile};
                        output.addObject(nativeTarget);
                        targetIds.push_back(nativeTarget.getId());
                    }
                }

                PbxProject pbxProject{project.getOrganization(),
                    projectConfigurationList,
                    mainGroup,
                    productRefGroup,
                    {{"ProductGroup", ouzelPoductRefGroup.getId()},
                     {"ProjectRef", ouzelProjectFileRef.getId()}},
                    targetIds};
                output.addRootObject(pbxProject);

                // TODO: resource build phases (PBXResourcesBuildPhase)
                // TODO: shell script (PBXShellScriptBuildPhase)

                std::ofstream file(projectFile, std::ios::trunc);
                file << output.encode();
            }

        private:
            class Output final
            {
            public:
                void addObject(const PbxObject& object)
                {
                    root["objects"][toString(object.getId())] = object.encode();
                }

                void addRootObject(const PbxObject& object)
                {
                    root["objects"][toString(object.getId())] = object.encode();
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
}

#endif // OUZEL_XCODE_PROJECT_HPP
