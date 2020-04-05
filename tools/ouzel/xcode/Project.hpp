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

                const auto& quartzCoreRef = create<PbxFileReference>("QuartzCore.framework",
                                                                     storage::Path{"System/Library/Frameworks/QuartzCore.framework"},
                                                                     "wrapper.framework", PbxSourceTree::SdkRoot);

                const auto ouzelProjectPath = project.getOuzelPath() / "build" / "ouzel.xcodeproj";
                const auto& ouzelProjectFileRef = create<PbxFileReference>("ouzel.xcodeproj", ouzelProjectPath,
                                                                           "wrapper.pb-project", PbxSourceTree::Group);
                const auto& libouzelIosProxy = create<PbxContainerItemProxy>(ouzelProjectFileRef,
                                                                             libouzelIosId, "libouzel_ios");

                const auto& libouzelIosReferenceProxy = create<PbxReferenceProxy>("", "libouzel_ios.a",
                                                                                  "archive.ar",
                                                                                  PbxSourceTree::BuildProductsDir,
                                                                                  libouzelIosProxy);

                const auto& libouzelMacOsProxy = create<PbxContainerItemProxy>(ouzelProjectFileRef,
                                                                               libouzelMacOsId, "libouzel_macos");

                const auto& libouzelMacOsReferenceProxy = create<PbxReferenceProxy>("", "libouzel_macos.a",
                                                                                    "archive.ar",
                                                                                    PbxSourceTree::BuildProductsDir,
                                                                                    libouzelMacOsProxy);

                const auto& libouzelTvosProxy = create<PbxContainerItemProxy>(ouzelProjectFileRef,
                                                                              libouzelTvosId, "libouzel_tvos");

                const auto& libouzelTvosReferenceProxy = create<PbxReferenceProxy>("", "libouzel_tvos.a",
                                                                                   "archive.ar",
                                                                                   PbxSourceTree::BuildProductsDir,
                                                                                   libouzelTvosProxy);

                const auto& ouzelProxy = create<PbxContainerItemProxy>(ouzelProjectFileRef,
                                                                       ouzelId, "ouzel");

                const auto& ouzelReferenceProxy = create<PbxReferenceProxy>("", "ouzel",
                                                                            "compiled.mach-o.executable",
                                                                            PbxSourceTree::BuildProductsDir,
                                                                            ouzelProxy);

                const auto& frameworksGroup = create<PbxGroup>("Frameworks", storage::Path{},
                                                               std::vector<PbxFileElementRef>{quartzCoreRef},
                                                               PbxSourceTree::Group);

                const auto& ouzelPoductRefGroup = create<PbxGroup>("Products", storage::Path{},
                                                                   std::vector<PbxFileElementRef>{
                                                                       libouzelIosReferenceProxy,
                                                                       libouzelMacOsReferenceProxy,
                                                                       libouzelTvosReferenceProxy,
                                                                       ouzelReferenceProxy},
                                                                   PbxSourceTree::Group);

                const auto& productFile = create<PbxFileReference>("", storage::Path{project.getName() + ".app"},
                                                                   "wrapper.application",
                                                                   PbxSourceTree::BuildProductsDir);

                const auto& productRefGroup = create<PbxGroup>("Products", storage::Path{},
                                                               std::vector<PbxFileElementRef>{productFile},
                                                               PbxSourceTree::Group);

                std::vector<PbxBuildFileRef> buildFiles;
                std::vector<PbxFileElementRef> sourceFiles;

                for (const auto& sourceFile : project.getSourceFiles())
                {
                    const auto extension = sourceFile.getExtension();
                    const std::string fileType = extension == "plist" ?
                        "text.plist.xml" :
                        "sourcecode.cpp." + extension;

                    const auto& fileReference = create<PbxFileReference>("", sourceFile, fileType, PbxSourceTree::Group);
                    sourceFiles.push_back(fileReference);

                    const auto& buildFile = create<PbxBuildFile>(fileReference);
                    buildFiles.push_back(buildFile);
                }

                const auto& sourceGroup = create<PbxGroup>("src", storage::Path{"src"},
                                                           sourceFiles, PbxSourceTree::Group);

                const auto& mainGroup = create<PbxGroup>("", storage::Path{},
                                                         std::vector<PbxFileElementRef>{
                                                             frameworksGroup,
                                                             ouzelProjectFileRef,
                                                             productRefGroup,
                                                             sourceGroup},
                                                         PbxSourceTree::Group);

                const auto headerSearchPath = std::string(project.getOuzelPath() / "engine");
                const auto& debugConfiguration = create<XcBuildConfiguration>("Debug",
                                                                              std::map<std::string, std::string>{
                                                                                  {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"},
                                                                                  {"GCC_OPTIMIZATION_LEVEL", "0"},
                                                                                  {"HEADER_SEARCH_PATHS", headerSearchPath}});

                const auto& releaseConfiguration = create<XcBuildConfiguration>("Release",
                                                                                std::map<std::string, std::string>{
                                                                                    {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"},
                                                                                    {"HEADER_SEARCH_PATHS", headerSearchPath}});

                const auto& projectConfigurationList = create<XcConfigurationList>(std::vector<XcBuildConfigurationRef>{
                                                                                       debugConfiguration,
                                                                                       releaseConfiguration},
                                                                                   releaseConfiguration.getName());

                std::vector<PbxTargetRef> targets;

                for (const auto platform : project.getPlatforms())
                {
                    // TODO: do it for all platforms
                    if (platform == Platform::MacOs)
                    {
                        const auto& targetDebugConfiguration = create<XcBuildConfiguration>("Debug",
                                                                                            std::map<std::string, std::string>{
                                                                                                {"PRODUCT_NAME", project.getName()}});

                        const auto& targetReleaseConfiguration = create<XcBuildConfiguration>("Release",
                                                                                              std::map<std::string, std::string>{
                                                                                                  {"PRODUCT_NAME", project.getName()}});

                        const auto& targetConfigurationList = create<XcConfigurationList>(std::vector<XcBuildConfigurationRef>{
                                                                                              targetDebugConfiguration,
                                                                                              targetReleaseConfiguration},
                                                                                          targetReleaseConfiguration.getName());

                        const auto& sourcesBuildPhase = create<PbxSourcesBuildPhase>(buildFiles);
                        const auto& quartzCoreBuildFile = create<PbxBuildFile>(quartzCoreRef);
                        const auto& libouzelMacOsBuildFile = create<PbxBuildFile>(libouzelMacOsReferenceProxy);

                        const auto& frameworksBuildPhase = create<PbxFrameworksBuildPhase>(std::vector<PbxBuildFileRef>{quartzCoreBuildFile, libouzelMacOsBuildFile});

                        const auto& nativeTarget = create<PbxNativeTarget>(project.getName() + " macOS",
                                                                           targetConfigurationList,
                                                                           std::vector<PbxBuildPhaseRef>{sourcesBuildPhase, frameworksBuildPhase},
                                                                           productFile);
                        targets.push_back(nativeTarget);
                    }
                }

                const auto& pbxProject = create<PbxProject>(project.getOrganization(),
                                                            projectConfigurationList,
                                                            mainGroup,
                                                            productRefGroup,
                                                            std::map<std::string, PbxObjectRef>{
                                                                {"ProductGroup", ouzelPoductRefGroup},
                                                                {"ProjectRef", ouzelProjectFileRef}},
                                                            targets);

                rootObject = &pbxProject;

                // TODO: resource build phases (PBXResourcesBuildPhase)
                // TODO: shell script (PBXShellScriptBuildPhase)

                std::ofstream file(projectFile, std::ios::trunc);
                file << plist::encode(encode());
            }

        private:
            template <class T, class ...Args>
            T& create(Args&& ...args)
            {
                std::unique_ptr<T> object = std::make_unique<T>(args...);
                T& result = *object;
                objects.push_back(std::move(object));
                return result;
            }

            plist::Value encode() const {
                plist::Value result = plist::Value::Dictionary{
                    {"archiveVersion", 1},
                    {"classes", plist::Value::Dictionary{}},
                    {"objectVersion", 50},
                    {"objects", plist::Value::Dictionary{}},
                    {"rootObject", rootObject ? toString(rootObject->getId()): ""}
                };

                for (const auto& object : objects)
                    result["objects"][toString(object->getId())] = object->encode();

                return result;
            }

            const OuzelProject& project;
            std::vector<std::unique_ptr<PbxObject>> objects;
            const PbxObject* rootObject;
        };
    }
}

#endif // OUZEL_XCODE_PROJECT_HPP
