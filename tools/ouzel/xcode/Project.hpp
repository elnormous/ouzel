// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PROJECT_HPP
#define OUZEL_XCODE_PROJECT_HPP

#include <fstream>
#include "OuzelProject.hpp"
#include "PBXBuildFile.hpp"
#include "PBXContainerItemProxy.hpp"
#include "PBXFileReference.hpp"
#include "PBXFrameworksBuildPhase.hpp"
#include "PBXGroup.hpp"
#include "PBXNativeTarget.hpp"
#include "PBXProject.hpp"
#include "PBXReferenceProxy.hpp"
#include "PBXResourcesBuildPhase.hpp"
#include "PBXShellScriptBuildPhase.hpp"
#include "PBXSourcesBuildPhase.hpp"
#include "PBXTargetDependency.hpp"
#include "XCBuildConfiguration.hpp"
#include "XCConfigurationList.hpp"
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
                const storage::Path projectFilename = project.getPath().getFilename();
                const storage::Path projectDirectory = project.getPath().getDirectory();

                auto xcodeProjectDirectory = projectDirectory / storage::Path{project.getName() + ".xcodeproj"};
                auto xcodeProjectDirectoryType = xcodeProjectDirectory.getType();

                if (xcodeProjectDirectoryType == storage::Path::Type::NotFound)
                    storage::FileSystem::createDirectory(xcodeProjectDirectory);
                else if (xcodeProjectDirectoryType != storage::Path::Type::Directory)
                {
                    storage::FileSystem::deleteFile(xcodeProjectDirectory);
                    storage::FileSystem::createDirectory(xcodeProjectDirectory);
                }

                auto pbxProjectFile = xcodeProjectDirectory / storage::Path{"project.pbxproj"};

                constexpr auto libouzelIosId = Id{0x30, 0x3B, 0x75, 0x33, 0x1C, 0x2A, 0x3C, 0x58, 0x00, 0xFE, 0xDE, 0x92};
                constexpr auto libouzelMacOsId = Id{0x30, 0xA3, 0x96, 0x29, 0x24, 0x37, 0x73, 0xB5, 0x00, 0xD8, 0xE2, 0x8E};
                constexpr auto libouzelTvosId = Id{0x30, 0xA3, 0x96, 0x29, 0x24, 0x37, 0x73, 0xB5, 0x00, 0xD8, 0xE2, 0x8E};
                constexpr auto ouzelId = Id{0x30, 0xA3, 0x96, 0x29, 0x24, 0x37, 0x73, 0xB5, 0x00, 0xD8, 0xE2, 0x8E};

                const auto ouzelProjectPath = project.getOuzelPath() / "build" / "ouzel.xcodeproj";
                const auto& ouzelProjectFileRef = create<PBXFileReference>("ouzel.xcodeproj", ouzelProjectPath,
                                                                           PBXFileType::WrapperPBProject,
                                                                           PBXSourceTree::Group);
                const auto& libouzelIosProxy = create<PBXContainerItemProxy>(ouzelProjectFileRef,
                                                                             PBXContainerItemProxy::Reference,
                                                                             libouzelIosId, "libouzel_ios");

                const auto& libouzelIosReferenceProxy = create<PBXReferenceProxy>("", "libouzel_ios.a",
                                                                                  PBXFileType::ArchiveAr,
                                                                                  PBXSourceTree::BuildProductsDir,
                                                                                  libouzelIosProxy);

                const auto& libouzelMacOsProxy = create<PBXContainerItemProxy>(ouzelProjectFileRef,
                                                                               PBXContainerItemProxy::Reference,
                                                                               libouzelMacOsId, "libouzel_macos");

                const auto& libouzelMacOsReferenceProxy = create<PBXReferenceProxy>("", "libouzel_macos.a",
                                                                                    PBXFileType::ArchiveAr,
                                                                                    PBXSourceTree::BuildProductsDir,
                                                                                    libouzelMacOsProxy);

                const auto& libouzelTvosProxy = create<PBXContainerItemProxy>(ouzelProjectFileRef,
                                                                              PBXContainerItemProxy::Reference,
                                                                              libouzelTvosId, "libouzel_tvos");

                const auto& libouzelTvosReferenceProxy = create<PBXReferenceProxy>("", "libouzel_tvos.a",
                                                                                   PBXFileType::ArchiveAr,
                                                                                   PBXSourceTree::BuildProductsDir,
                                                                                   libouzelTvosProxy);

                const auto& ouzelProxy = create<PBXContainerItemProxy>(ouzelProjectFileRef,
                                                                       PBXContainerItemProxy::Reference,
                                                                       ouzelId, "ouzel");

                const auto& ouzelNativeTargetProxy = create<PBXContainerItemProxy>(ouzelProjectFileRef,
                                                                                   PBXContainerItemProxy::NativeTarget,
                                                                                   ouzelId, "ouzel");

                const auto& ouzelReferenceProxy = create<PBXReferenceProxy>("", "ouzel",
                                                                            PBXFileType::CompiledMachOExecutable,
                                                                            PBXSourceTree::BuildProductsDir,
                                                                            ouzelProxy);

                const auto& ouzelPoductRefGroup = create<PBXGroup>("Products", storage::Path{},
                                                                   std::vector<PBXFileElementRef>{
                                                                       libouzelIosReferenceProxy,
                                                                       libouzelMacOsReferenceProxy,
                                                                       libouzelTvosReferenceProxy,
                                                                       ouzelReferenceProxy},
                                                                   PBXSourceTree::Group);

                const auto& productFile = create<PBXFileReference>("", storage::Path{project.getName() + ".app"},
                                                                   PBXFileType::WrapperApplication,
                                                                   PBXSourceTree::BuildProductsDir);

                const auto& resourcesGroup = create<PBXGroup>("", storage::Path{"Resources"},
                                                              std::vector<PBXFileElementRef>{},
                                                              PBXSourceTree::Group);

                const auto& productRefGroup = create<PBXGroup>("Products", storage::Path{},
                                                               std::vector<PBXFileElementRef>{productFile},
                                                               PBXSourceTree::Group);

                std::vector<PBXBuildFileRef> buildFiles;
                std::vector<PBXFileElementRef> sourceFiles;

                for (const auto& sourceFile : project.getSourceFiles())
                {
                    const auto extension = sourceFile.getExtension();
                    // TODO: support more file formats
                    const auto fileType = extension == "plist" ? PBXFileType::TextPlistXml :
                        extension == "c" ? PBXFileType::SourcecodeC :
                        extension == "h" ? PBXFileType::SourcecodeCH :
                        extension == "cpp" ? PBXFileType::SourcecodeCppCpp :
                        extension == "hpp" ? PBXFileType::SourcecodeCppH :
                        throw std::runtime_error("Unsupported file type");

                    const auto& fileReference = create<PBXFileReference>("", sourceFile, fileType, PBXSourceTree::Group);
                    sourceFiles.push_back(fileReference);

                    const auto& buildFile = create<PBXBuildFile>(fileReference);
                    buildFiles.push_back(buildFile);
                }

                const auto& sourceGroup = create<PBXGroup>("src", storage::Path{"src"},
                                                           sourceFiles, PBXSourceTree::Group);

                const auto headerSearchPath = std::string(project.getOuzelPath() / "engine");
                const auto& debugConfiguration = create<XCBuildConfiguration>("Debug",
                                                                              std::map<std::string, std::string>{
                                                                                  {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"},
                                                                                  {"CLANG_CXX_LIBRARY", "libc++"},
                                                                                  {"ENABLE_TESTABILITY", "YES"},
                                                                                  {"GCC_OPTIMIZATION_LEVEL", "0"},
                                                                                  {"GCC_PREPROCESSOR_DEFINITIONS", "DEBUG=1"},
                                                                                  {"HEADER_SEARCH_PATHS", headerSearchPath},
                                                                                  {"ONLY_ACTIVE_ARCH", "YES"}});

                const auto& releaseConfiguration = create<XCBuildConfiguration>("Release",
                                                                                std::map<std::string, std::string>{
                                                                                    {"CLANG_CXX_LANGUAGE_STANDARD", "c++14"},
                                                                                    {"CLANG_CXX_LIBRARY", "libc++"},
                                                                                    {"HEADER_SEARCH_PATHS", headerSearchPath}});

                const auto& projectConfigurationList = create<XCConfigurationList>(std::vector<XCBuildConfigurationRef>{
                                                                                       debugConfiguration,
                                                                                       releaseConfiguration},
                                                                                   releaseConfiguration.getName());

                std::vector<PBXTargetRef> targets;
                std::vector<PBXFileElementRef> frameworkFiles;

                for (const auto platform : project.getPlatforms())
                {
                    if (platform == Platform::MacOs ||
                        platform == Platform::Ios ||
                        platform == Platform::Tvos)
                    {
                        std::map<std::string, std::string> buildSettings = {{"PRODUCT_NAME", project.getName()}};
                        storage::Path sdkPath;
                        PBXSourceTree frameworkSourceTree = PBXSourceTree::SdkRoot;
                        std::vector<const char*> frameworks;
                        std::vector<PBXBuildFileRef> frameworkBuildFiles;

                        switch (platform)
                        {
                            case Platform::MacOs:
                            {
                                buildSettings["SDKROOT"] = "macosx";
                                buildSettings["MACOSX_DEPLOYMENT_TARGET"] = "10.8";
                                frameworks = {"AudioToolbox.framework",
                                    "AudioUnit.framework",
                                    "Cocoa.framework",
                                    "CoreAudio.framework",
                                    "CoreVideo.framework",
                                    "GameController.framework",
                                    "IOKit.framework",
                                    "Metal.framework",
                                    "OpenAL.framework",
                                    "OpenGL.framework",
                                    "QuartzCore.framework"};
                                const auto& libouzelMacOsBuildFile = create<PBXBuildFile>(libouzelMacOsReferenceProxy);
                                frameworkBuildFiles.push_back(libouzelMacOsBuildFile);
                                break;
                            }

                            case Platform::Ios:
                            {
                                buildSettings["SDKROOT"] = "iphoneos";
                                buildSettings["IPHONEOS_DEPLOYMENT_TARGET"] = "8.0";
                                frameworks = {"AudioToolbox.framework",
                                    "AVFoundation.framework",
                                    "Foundation.framework",
                                    "GameController.framework",
                                    "Metal.framework",
                                    "OpenAL.framework",
                                    "OpenGLES.framework",
                                    "QuartzCore.framework",
                                    "UIKit.framework"
                                };
                                sdkPath = "Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk";
                                frameworkSourceTree = PBXSourceTree::DeveloperDir;
                                const auto& libouzelIosBuildFile = create<PBXBuildFile>(libouzelIosReferenceProxy);
                                frameworkBuildFiles.push_back(libouzelIosBuildFile);
                                break;
                            }

                            case Platform::Tvos:
                            {
                                buildSettings["SDKROOT"] = "appletvos";
                                buildSettings["TVOS_DEPLOYMENT_TARGET"] = "9.0";
                                frameworks = {"AudioToolbox.framework",
                                    "AVFoundation.framework",
                                    "Foundation.framework",
                                    "GameController.framework",
                                    "Metal.framework",
                                    "OpenAL.framework",
                                    "OpenGLES.framework",
                                    "QuartzCore.framework",
                                    "UIKit.framework"
                                };
                                sdkPath = "Platforms/AppleTVOS.platform/Developer/SDKs/AppleTVOS.sdk";
                                frameworkSourceTree = PBXSourceTree::DeveloperDir;
                                const auto& libouzelTvosBuildFile = create<PBXBuildFile>(libouzelTvosReferenceProxy);
                                frameworkBuildFiles.push_back(libouzelTvosBuildFile);
                                break;
                            }

                            default:
                                throw std::runtime_error("Unsupported platform");
                        }

                        const auto& targetDebugConfiguration = create<XCBuildConfiguration>("Debug", buildSettings);

                        const auto& targetReleaseConfiguration = create<XCBuildConfiguration>("Release", buildSettings);

                        const auto& targetConfigurationList = create<XCConfigurationList>(std::vector<XCBuildConfigurationRef>{
                                                                                              targetDebugConfiguration,
                                                                                              targetReleaseConfiguration},
                                                                                          targetReleaseConfiguration.getName());

                        const auto& sourcesBuildPhase = create<PBXSourcesBuildPhase>(buildFiles);

                        std::vector<PBXFileReferenceRef> frameworkFileReferences;

                        const auto frameworksPath = sdkPath / storage::Path{"System/Library/Frameworks"};
                        for (const auto& framework : frameworks)
                        {
                            const auto& frameworkFileReference = create<PBXFileReference>(framework,
                                                                                          frameworksPath / framework,
                                                                                          PBXFileType::WrapperFramework,
                                                                                          frameworkSourceTree);

                            frameworkFileReferences.push_back(frameworkFileReference);
                            frameworkFiles.push_back(frameworkFileReference);
                        }

                        for (const PBXFileReferenceRef& frameworkFileReference : frameworkFileReferences)
                        {
                            const auto& frameworkBuildFile = create<PBXBuildFile>(frameworkFileReference);
                            frameworkBuildFiles.push_back(frameworkBuildFile);
                        }

                        const auto& frameworksBuildPhase = create<PBXFrameworksBuildPhase>(frameworkBuildFiles);

                        const auto& assetsBuildPhase = create<PBXShellScriptBuildPhase>("$BUILT_PRODUCTS_DIR/ouzel --export-assets $PROJECT_DIR/" + std::string(projectFilename));

                        // TODO: implement resource copy
                        const auto& resourcesBuildPhase = create<PBXResourcesBuildPhase>(std::vector<PBXBuildFileRef>{});

                        const auto& ouzelDependency = create<PBXTargetDependency>("ouzel", ouzelNativeTargetProxy);

                        const auto& nativeTarget = create<PBXNativeTarget>(project.getName() + ' ' + toString(platform),
                                                                           targetConfigurationList,
                                                                           std::vector<PBXBuildPhaseRef>{sourcesBuildPhase, frameworksBuildPhase, assetsBuildPhase, resourcesBuildPhase},
                                                                           std::vector<PBXTargetDependencyRef>{ouzelDependency},
                                                                           productFile);
                        targets.push_back(nativeTarget);
                    }
                }

                const auto& frameworksGroup = create<PBXGroup>("Frameworks", storage::Path{},
                                                               frameworkFiles,
                                                               PBXSourceTree::Group);

                const auto& mainGroup = create<PBXGroup>("", storage::Path{},
                                                         std::vector<PBXFileElementRef>{
                                                             frameworksGroup,
                                                             ouzelProjectFileRef,
                                                             productRefGroup,
                                                             resourcesGroup,
                                                             sourceGroup},
                                                         PBXSourceTree::Group);

                const auto& pbxProject = create<PBXProject>(project.getOrganization(),
                                                            projectConfigurationList,
                                                            mainGroup,
                                                            productRefGroup,
                                                            std::map<std::string, PBXObjectRef>{
                                                                {"ProductGroup", ouzelPoductRefGroup},
                                                                {"ProjectRef", ouzelProjectFileRef}},
                                                            targets);

                rootObject = &pbxProject;

                std::ofstream file(pbxProjectFile, std::ios::trunc);
                file << plist::encode(encode());
            }

        private:
            template <class T, class ...Args>
            T& create(Args&& ...args)
            {
                std::unique_ptr<T> object = std::make_unique<T>(std::forward<Args>(args)...);
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
            std::vector<std::unique_ptr<PBXObject>> objects;
            const PBXObject* rootObject;
        };
    }
}

#endif // OUZEL_XCODE_PROJECT_HPP
