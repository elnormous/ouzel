// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_XCODEPROJECT_HPP
#define OUZEL_XCODE_XCODEPROJECT_HPP

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "../Project.hpp"
#include "PBXBuildFile.hpp"
#include "PBXContainerItemProxy.hpp"
#include "PBXFileReference.hpp"
#include "PBXFrameworksBuildPhase.hpp"
#include "PBXGroup.hpp"
#include "PBXLegacyTarget.hpp"
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
#include "formats/Plist.hpp"

namespace ouzel::xcode
{
    class Project final
    {
    public:
        Project(const ouzel::Project& project)
        {
            const auto projectFilename = project.getPath().getFilename();
            const auto projectDirectory = project.getPath().getDirectory();

            auto pbxProject = alloc<PBXProject>();
            pbxProject->organization = project.getOrganization();
            rootObject = pbxProject;

            auto mainGroup = alloc<PBXGroup>();
            mainGroup->sourceTree = PBXSourceTree::group;
            pbxProject->mainGroup = mainGroup;

            auto engineProjectFileRef = alloc<PBXFileReference>();
            engineProjectFileRef->name = "libouzel.xcodeproj";
            engineProjectFileRef->path = project.getOuzelPath() / "engine" / "libouzel.xcodeproj";
            engineProjectFileRef->fileType = PBXFileType::wrapperPBProject;
            engineProjectFileRef->sourceTree = PBXSourceTree::group;
            mainGroup->children.push_back(engineProjectFileRef);

            auto enginePoductRefGroup = alloc<PBXGroup>();
            enginePoductRefGroup->name = "Products";
            enginePoductRefGroup->path = storage::Path{};
            enginePoductRefGroup->sourceTree = PBXSourceTree::group;

            pbxProject->projectReferences.push_back({
                {"ProjectRef", engineProjectFileRef},
                {"ProductGroup", enginePoductRefGroup}
            });

            PBXObject libouzelIos(Id{0x30, 0x3B, 0x75, 0x33, 0x1C, 0x2A, 0x3C, 0x58, 0x00, 0xFE, 0xDE, 0x92});

            auto libouzelIosProxy = alloc<PBXContainerItemProxy>();
            libouzelIosProxy->containerPortal = engineProjectFileRef;
            libouzelIosProxy->proxyType = PBXContainerItemProxy::ProxyType::reference;
            libouzelIosProxy->remoteGlobal = &libouzelIos;
            libouzelIosProxy->remoteInfo = "libouzel_ios";

            auto libouzelIosReferenceProxy = alloc<PBXReferenceProxy>();
            libouzelIosReferenceProxy->path = "libouzel_ios.a";
            libouzelIosReferenceProxy->fileType = PBXFileType::archiveAr;
            libouzelIosReferenceProxy->sourceTree = PBXSourceTree::buildProductsDir;
            libouzelIosReferenceProxy->remoteRef = libouzelIosProxy;
            enginePoductRefGroup->children.push_back(libouzelIosReferenceProxy);

            PBXObject libouzelMacOs(Id{0x30, 0x4A, 0x8E, 0x25, 0x1C, 0x23, 0x7C, 0x30, 0x00, 0x8B, 0x11, 0x51});

            auto libouzelMacOsProxy = alloc<PBXContainerItemProxy>();
            libouzelMacOsProxy->containerPortal = engineProjectFileRef;
            libouzelMacOsProxy->proxyType = PBXContainerItemProxy::ProxyType::reference;
            libouzelMacOsProxy->remoteGlobal = &libouzelMacOs;
            libouzelMacOsProxy->remoteInfo = "libouzel_macos";

            auto libouzelMacOsReferenceProxy = alloc<PBXReferenceProxy>();
            libouzelMacOsReferenceProxy->path = "libouzel_macos.a";
            libouzelMacOsReferenceProxy->fileType = PBXFileType::archiveAr;
            libouzelMacOsReferenceProxy->sourceTree = PBXSourceTree::buildProductsDir;
            libouzelMacOsReferenceProxy->remoteRef = libouzelMacOsProxy;
            enginePoductRefGroup->children.push_back(libouzelMacOsReferenceProxy);

            PBXObject libouzelTvos(Id{0x30, 0x3B, 0x76, 0x80, 0x1C, 0x35, 0x5A, 0x3B, 0x00, 0xFE, 0xDE, 0x92});

            auto libouzelTvosProxy = alloc<PBXContainerItemProxy>();
            libouzelTvosProxy->containerPortal = engineProjectFileRef;
            libouzelTvosProxy->proxyType = PBXContainerItemProxy::ProxyType::reference;
            libouzelTvosProxy->remoteGlobal = &libouzelTvos;
            libouzelTvosProxy->remoteInfo = "libouzel_tvos";

            auto libouzelTvosReferenceProxy = alloc<PBXReferenceProxy>();
            libouzelTvosReferenceProxy->path = "libouzel_tvos.a";
            libouzelTvosReferenceProxy->fileType = PBXFileType::archiveAr;
            libouzelTvosReferenceProxy->sourceTree = PBXSourceTree::buildProductsDir;
            libouzelTvosReferenceProxy->remoteRef = libouzelTvosProxy;
            enginePoductRefGroup->children.push_back(libouzelTvosReferenceProxy);

            auto ouzelProjectFileRef = alloc<PBXFileReference>();
            ouzelProjectFileRef->name = "ouzel.xcodeproj";
            ouzelProjectFileRef->path = project.getOuzelPath() / "tools" / "ouzel.xcodeproj";
            ouzelProjectFileRef->fileType = PBXFileType::wrapperPBProject;
            ouzelProjectFileRef->sourceTree = PBXSourceTree::group;
            mainGroup->children.push_back(ouzelProjectFileRef);

            auto ouzelPoductRefGroup = alloc<PBXGroup>();
            ouzelPoductRefGroup->name = "Products";
            ouzelPoductRefGroup->path = storage::Path{};
            ouzelPoductRefGroup->sourceTree = PBXSourceTree::group;

            pbxProject->projectReferences.push_back({
                {"ProjectRef", ouzelProjectFileRef},
                {"ProductGroup", ouzelPoductRefGroup}
            });

            PBXObject ouzel(Id{0x30, 0x23, 0x20, 0x0C, 0x22, 0x22, 0x0B, 0xCF, 0x00, 0x7E, 0x0A, 0xAD});

            auto ouzelProxy = alloc<PBXContainerItemProxy>();
            ouzelProxy->containerPortal = ouzelProjectFileRef;
            ouzelProxy->proxyType = PBXContainerItemProxy::ProxyType::reference;
            ouzelProxy->remoteGlobal = &ouzel;
            ouzelProxy->remoteInfo = "ouzel";

            auto productFile = alloc<PBXFileReference>();
            productFile->path = storage::Path{project.getName() + ".app"};
            productFile->fileType = PBXFileType::wrapperApplication;
            productFile->sourceTree = PBXSourceTree::buildProductsDir;

            auto resourcesGroup = alloc<PBXGroup>();
            resourcesGroup->path = storage::Path{"Resources"};
            resourcesGroup->sourceTree = PBXSourceTree::group;
            mainGroup->children.push_back(resourcesGroup);

            auto productRefGroup = alloc<PBXGroup>();
            productRefGroup->name = "Products";
            productRefGroup->children.push_back(productFile);
            productRefGroup->sourceTree = PBXSourceTree::group;
            mainGroup->children.push_back(productRefGroup);
            pbxProject->productRefGroup = productRefGroup;

            std::vector<const PBXBuildFile*> buildFiles;
            std::vector<const PBXFileElement*> sourceFiles;

            for (const auto& sourceFile : project.getSourceFiles())
            {
                const auto extension = sourceFile.getExtension();

                auto fileReference = alloc<PBXFileReference>();
                fileReference->path = sourceFile;
                // TODO: support more file formats
                fileReference->fileType = extension == "plist" ? PBXFileType::textPlistXml :
                    extension == "c" ? PBXFileType::sourcecodeC :
                    extension == "h" ? PBXFileType::sourcecodeCH :
                    extension == "cpp" ? PBXFileType::sourcecodeCppCpp :
                    extension == "hpp" ? PBXFileType::sourcecodeCppH :
                    throw std::runtime_error("Unsupported file type");
                fileReference->sourceTree = PBXSourceTree::group;
                sourceFiles.push_back(fileReference);

                auto buildFile = alloc<PBXBuildFile>();
                buildFile->fileRef = fileReference;
                buildFiles.push_back(buildFile);
            }

            auto frameworksGroup = alloc<PBXGroup>();
            frameworksGroup->name = "Frameworks";
            frameworksGroup->sourceTree = PBXSourceTree::group;
            mainGroup->children.push_back(frameworksGroup);

            auto sourceGroup = alloc<PBXGroup>();
            sourceGroup->name = "src";
            sourceGroup->path = storage::Path{"src"};
            sourceGroup->children = sourceFiles;
            sourceGroup->sourceTree = PBXSourceTree::group;
            mainGroup->children.push_back(sourceGroup);

            auto projectConfigurationList = alloc<XCConfigurationList>();
            pbxProject->buildConfigurationList = projectConfigurationList;

            const auto headerSearchPath = std::string(project.getOuzelPath() / "engine");
            auto debugConfiguration = alloc<XCBuildConfiguration>();
            debugConfiguration->name = "Debug";
            debugConfiguration->buildSettings = {
                {"CLANG_CXX_LANGUAGE_STANDARD", "c++17"},
                {"CLANG_CXX_LIBRARY", "libc++"},
                {"ENABLE_TESTABILITY", "YES"},
                {"GCC_OPTIMIZATION_LEVEL", "0"},
                {"GCC_PREPROCESSOR_DEFINITIONS", "DEBUG"},
                {"HEADER_SEARCH_PATHS", headerSearchPath},
                {"ONLY_ACTIVE_ARCH", "YES"},
                {"CLANG_WARN_ASSIGN_ENUM", "YES"},
                {"CLANG_WARN_COMMA", "YES"},
                {"CLANG_WARN_EMPTY_BODY", "YES"},
                {"CLANG_WARN_IMPLICIT_SIGN_CONVERSION", "YES"},
                {"CLANG_WARN_INFINITE_RECURSION", "YES"},
                {"CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER", "YES"},
                {"CLANG_WARN_SEMICOLON_BEFORE_METHOD_BODY", "YES"},
                {"CLANG_WARN_STRICT_PROTOTYPES", "YES"},
                {"CLANG_WARN_SUSPICIOUS_IMPLICIT_CONVERSION", "YES"},
                {"CLANG_WARN_SUSPICIOUS_MOVE", "YES"},
                {"CLANG_WARN_UNREACHABLE_CODE", "YES"},
                {"GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS", "YES"},
                {"GCC_WARN_ABOUT_MISSING_NEWLINE", "YES"},
                {"GCC_WARN_ABOUT_MISSING_PROTOTYPES", "YES"},
                {"GCC_WARN_ABOUT_RETURN_TYPE", "YES"},
                {"GCC_WARN_FOUR_CHARACTER_CONSTANTS", "YES"},
                {"GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS", "YES"},
                {"GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED", "YES"},
                {"GCC_WARN_PEDANTIC", "YES"},
                {"GCC_WARN_SHADOW", "YES"},
                {"GCC_WARN_SIGN_COMPARE", "YES"},
                {"GCC_WARN_UNINITIALIZED_AUTOS", "YES_AGGRESSIVE"},
                {"GCC_WARN_UNKNOWN_PRAGMAS", "YES"},
                {"GCC_WARN_UNUSED_FUNCTION", "YES"},
                {"GCC_WARN_UNUSED_LABEL", "YES"},
                {"GCC_WARN_UNUSED_PARAMETER", "YES"},
                {"GCC_WARN_UNUSED_VARIABLE", "YES"}
            };

            projectConfigurationList->configurations.push_back(debugConfiguration);

            auto releaseConfiguration = alloc<XCBuildConfiguration>();
            releaseConfiguration->name = "Release";
            releaseConfiguration->buildSettings = {
                {"CLANG_CXX_LANGUAGE_STANDARD", "c++17"},
                {"CLANG_CXX_LIBRARY", "libc++"},
                {"HEADER_SEARCH_PATHS", headerSearchPath},
                {"CLANG_WARN_ASSIGN_ENUM", "YES"},
                {"CLANG_WARN_COMMA", "YES"},
                {"CLANG_WARN_EMPTY_BODY", "YES"},
                {"CLANG_WARN_IMPLICIT_SIGN_CONVERSION", "YES"},
                {"CLANG_WARN_INFINITE_RECURSION", "YES"},
                {"CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER", "YES"},
                {"CLANG_WARN_SEMICOLON_BEFORE_METHOD_BODY", "YES"},
                {"CLANG_WARN_STRICT_PROTOTYPES", "YES"},
                {"CLANG_WARN_SUSPICIOUS_IMPLICIT_CONVERSION", "YES"},
                {"CLANG_WARN_SUSPICIOUS_MOVE", "YES"},
                {"CLANG_WARN_UNREACHABLE_CODE", "YES"},
                {"GCC_WARN_ABOUT_MISSING_FIELD_INITIALIZERS", "YES"},
                {"GCC_WARN_ABOUT_MISSING_NEWLINE", "YES"},
                {"GCC_WARN_ABOUT_MISSING_PROTOTYPES", "YES"},
                {"GCC_WARN_ABOUT_RETURN_TYPE", "YES"},
                {"GCC_WARN_FOUR_CHARACTER_CONSTANTS", "YES"},
                {"GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS", "YES"},
                {"GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED", "YES"},
                {"GCC_WARN_PEDANTIC", "YES"},
                {"GCC_WARN_SHADOW", "YES"},
                {"GCC_WARN_SIGN_COMPARE", "YES"},
                {"GCC_WARN_UNINITIALIZED_AUTOS", "YES_AGGRESSIVE"},
                {"GCC_WARN_UNKNOWN_PRAGMAS", "YES"},
                {"GCC_WARN_UNUSED_FUNCTION", "YES"},
                {"GCC_WARN_UNUSED_LABEL", "YES"},
                {"GCC_WARN_UNUSED_PARAMETER", "YES"},
                {"GCC_WARN_UNUSED_VARIABLE", "YES"}
            };

            projectConfigurationList->configurations.push_back(releaseConfiguration);
            projectConfigurationList->defaultConfigurationName = releaseConfiguration->name;

            for (const auto& target : project.getTargets())
                if (target.platform == Platform::macOs ||
                    target.platform == Platform::iOs ||
                    target.platform == Platform::tvOs)
                {
                    const storage::Path targetPath{target.name};

                    auto nativeTarget = alloc<PBXNativeTarget>();
                    nativeTarget->name = target.name;
                    nativeTarget->productReference = productFile;
                    pbxProject->targets.push_back(nativeTarget);

                    auto targetConfigurationList = alloc<XCConfigurationList>();
                    nativeTarget->buildConfigurationList = targetConfigurationList;

                    auto targetDebugConfiguration = alloc<XCBuildConfiguration>();
                    targetDebugConfiguration->name = "Debug";
                    targetConfigurationList->configurations.push_back(targetDebugConfiguration);

                    auto targetReleaseConfiguration = alloc<XCBuildConfiguration>();
                    targetReleaseConfiguration->name = "Release";
                    targetConfigurationList->configurations.push_back(targetReleaseConfiguration);
                    targetConfigurationList->defaultConfigurationName = targetReleaseConfiguration->name;

                    auto sourcesBuildPhase = alloc<PBXSourcesBuildPhase>();
                    sourcesBuildPhase->files = buildFiles;
                    nativeTarget->buildPhases.push_back(sourcesBuildPhase);

                    auto frameworksBuildPhase = alloc<PBXFrameworksBuildPhase>();
                    nativeTarget->buildPhases.push_back(frameworksBuildPhase);

                    std::map<std::string, std::string> buildSettings = {
                        {"INFOPLIST_FILE", targetPath / "Info.plist"},
                        {"PRODUCT_BUNDLE_IDENTIFIER", project.getIdentifier()},
                        {"PRODUCT_NAME", project.getName()}
                    };

                    auto infoPlistFileReference = alloc<PBXFileReference>();
                    infoPlistFileReference->path = storage::Path{"Info.plist"};
                    infoPlistFileReference->fileType = PBXFileType::textPlistXml;
                    infoPlistFileReference->sourceTree = PBXSourceTree::group;

                    auto platformGroup = alloc<PBXGroup>();
                    platformGroup->path = targetPath;
                    platformGroup->children.push_back(infoPlistFileReference);
                    platformGroup->sourceTree = PBXSourceTree::group;
                    mainGroup->children.push_back(platformGroup);

                    storage::Path sdkPath;
                    PBXSourceTree frameworkSourceTree = PBXSourceTree::sdkRoot;
                    std::set<const char*> frameworks;

                    const auto platformDirectory = projectDirectory / targetPath;
                    const auto plistPath = platformDirectory / "Info.plist";
                    const auto platformDirectoryType = storage::FileSystem::getFileType(platformDirectory);
                    if (platformDirectoryType == storage::FileType::notFound)
                        storage::FileSystem::createDirectory(platformDirectory);
                    else if (platformDirectoryType != storage::FileType::directory)
                    {
                        storage::FileSystem::deleteFile(platformDirectory);
                        storage::FileSystem::createDirectory(platformDirectory);
                    }

                    switch (target.platform)
                    {
                        case Platform::macOs:
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
                            auto libouzelMacOsBuildFile = alloc<PBXBuildFile>();
                            libouzelMacOsBuildFile->fileRef = libouzelMacOsReferenceProxy;
                            frameworksBuildPhase->files.push_back(libouzelMacOsBuildFile);

                            plist::Value infoPlist = plist::Dictionary{
                                {"CFBundleDevelopmentRegion", "en"},
                                {"CFBundleExecutable", "$(EXECUTABLE_NAME)"},
                                {"CFBundleIconFile", ""},
                                {"CFBundleIdentifier", "$(PRODUCT_BUNDLE_IDENTIFIER)"},
                                {"CFBundleInfoDictionaryVersion", "6.0"},
                                {"CFBundleName", "$(PRODUCT_NAME)"},
                                {"CFBundlePackageType", "APPL"},
                                {"CFBundleShortVersionString", "1.0"},
                                {"CFBundleSignature", "????"},
                                {"CFBundleVersion", "1"},
                                {"LSMinimumSystemVersion", "$(MACOSX_DEPLOYMENT_TARGET)"},
                                {"NSHumanReadableCopyright", "Copyright " + project.getOrganization() + ". All rights reserved."},
                                {"NSPrincipalClass", "NSApplication"}
                            };

                            std::ofstream infoPlistFile(plistPath);
                            infoPlistFile << plist::encode(infoPlist, plist::Format::xml, true);
                            break;
                        }

                        case Platform::iOs:
                        {
                            buildSettings["SDKROOT"] = "iphoneos";
                            buildSettings["IPHONEOS_DEPLOYMENT_TARGET"] = "8.0";
                            buildSettings["TARGETED_DEVICE_FAMILY"] = "1,2";
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
                            frameworkSourceTree = PBXSourceTree::developerDir;
                            auto libouzelIosBuildFile = alloc<PBXBuildFile>();
                            libouzelIosBuildFile->fileRef = libouzelIosReferenceProxy;
                            frameworksBuildPhase->files.push_back(libouzelIosBuildFile);

                            plist::Value infoPlist = plist::Dictionary{
                                {"CFBundleDevelopmentRegion", "en"},
                                {"CFBundleExecutable", "$(EXECUTABLE_NAME)"},
                                {"CFBundleIdentifier", "$(PRODUCT_BUNDLE_IDENTIFIER)"},
                                {"CFBundleInfoDictionaryVersion", "6.0"},
                                {"CFBundleName", "$(PRODUCT_NAME)"},
                                {"CFBundlePackageType", "APPL"},
                                {"CFBundleShortVersionString", "1.0"},
                                {"CFBundleSignature", "????"},
                                {"CFBundleVersion", "1"},
                                {"LSRequiresIPhoneOS", true},
                                {"UILaunchStoryboardName", "LaunchScreen"},
                                {"UIRequiredDeviceCapabilities", plist::Array{"armv7"}},
                                {"UIRequiresFullScreen", true},
                                {"UIStatusBarHidden", true},
                                {"UISupportedInterfaceOrientations", plist::Array{
                                    "UIInterfaceOrientationPortrait",
                                    "UIInterfaceOrientationPortraitUpsideDown",
                                    "UIInterfaceOrientationLandscapeLeft",
                                    "UIInterfaceOrientationLandscapeRight"
                                }},
                                {"UISupportedInterfaceOrientations~ipad", plist::Array{
                                    "UIInterfaceOrientationPortrait",
                                    "UIInterfaceOrientationPortraitUpsideDown",
                                    "UIInterfaceOrientationLandscapeLeft",
                                    "UIInterfaceOrientationLandscapeRight"
                                }}
                            };

                            std::ofstream infoPlistFile(plistPath);
                            infoPlistFile << plist::encode(infoPlist, plist::Format::xml, true);
                            break;
                        }

                        case Platform::tvOs:
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
                            frameworkSourceTree = PBXSourceTree::developerDir;
                            auto libouzelTvosBuildFile = alloc<PBXBuildFile>();
                            libouzelTvosBuildFile->fileRef = libouzelTvosReferenceProxy;
                            frameworksBuildPhase->files.push_back(libouzelTvosBuildFile);

                            plist::Value infoPlist = plist::Dictionary{
                                {"CFBundleDevelopmentRegion", "en"},
                                {"CFBundleExecutable", "$(EXECUTABLE_NAME)"},
                                {"CFBundleIdentifier", "$(PRODUCT_BUNDLE_IDENTIFIER)"},
                                {"CFBundleInfoDictionaryVersion", "6.0"},
                                {"CFBundleName", "$(PRODUCT_NAME)"},
                                {"CFBundlePackageType", "APPL"},
                                {"CFBundleShortVersionString", "1.0"},
                                {"CFBundleSignature", "????"},
                                {"CFBundleVersion", "1"},
                                {"LSRequiresIPhoneOS", true},
                                {"UILaunchStoryboardName", "LaunchScreen"},
                                {"UIRequiredDeviceCapabilities", plist::Array{"armv7"}}
                            };

                            std::ofstream infoPlistFile(plistPath);
                            infoPlistFile << plist::encode(infoPlist, plist::Format::xml, true);
                            break;
                        }

                        default:
                            throw std::runtime_error("Unsupported platform");
                    }

                    targetDebugConfiguration->buildSettings = buildSettings;
                    targetReleaseConfiguration->buildSettings = buildSettings;

                    const auto frameworksPath = sdkPath / storage::Path{"System/Library/Frameworks"};
                    for (const auto framework : frameworks)
                    {
                        auto frameworkFileReference = alloc<PBXFileReference>();
                        frameworkFileReference->name = framework;
                        frameworkFileReference->path = frameworksPath / framework;
                        frameworkFileReference->fileType = PBXFileType::wrapperFramework;
                        frameworkFileReference->sourceTree = frameworkSourceTree;
                        frameworksGroup->children.push_back(frameworkFileReference);

                        auto frameworkBuildFile = alloc<PBXBuildFile>();
                        frameworkBuildFile->fileRef = frameworkFileReference;
                        frameworksBuildPhase->files.push_back(frameworkBuildFile);
                    }

                    // TODO: implement resource copy
                    auto resourcesBuildPhase = alloc<PBXResourcesBuildPhase>();
                    nativeTarget->buildPhases.push_back(resourcesBuildPhase);

                    auto ouzelNativeTargetProxy = alloc<PBXContainerItemProxy>();
                    ouzelNativeTargetProxy->containerPortal = ouzelProjectFileRef;
                    ouzelNativeTargetProxy->proxyType = PBXContainerItemProxy::ProxyType::nativeTarget;
                    ouzelNativeTargetProxy->remoteGlobal = &ouzel;
                    ouzelNativeTargetProxy->remoteInfo = "ouzel";

                    auto ouzelReferenceProxy = alloc<PBXReferenceProxy>();
                    ouzelReferenceProxy->path = "ouzel";
                    ouzelReferenceProxy->fileType = PBXFileType::compiledMachOExecutable;
                    ouzelReferenceProxy->sourceTree = PBXSourceTree::buildProductsDir;
                    ouzelReferenceProxy->remoteRef = ouzelProxy;

                    auto ouzelDependency = alloc<PBXTargetDependency>();
                    ouzelDependency->name = "ouzel";
                    ouzelDependency->targetProxy = ouzelNativeTargetProxy;

                    auto assetGenerateTarget = alloc<PBXLegacyTarget>();
                    assetGenerateTarget->name = "Generate Assets " + target.name;
                    assetGenerateTarget->buildToolPath = "$BUILT_PRODUCTS_DIR/ouzel";
                    assetGenerateTarget->buildArgumentsString = "--export-assets $PROJECT_DIR/" + escapeForShell(projectFilename) + " --target " + escapeForShell(target.name);
                    assetGenerateTarget->dependencies.push_back(ouzelDependency);
                    pbxProject->targets.push_back(assetGenerateTarget);

                    auto assetGenerateTargetConfigurationList = alloc<XCConfigurationList>();
                    assetGenerateTarget->buildConfigurationList = assetGenerateTargetConfigurationList;

                    auto assetGenerateTargetDebugConfiguration = alloc<XCBuildConfiguration>();
                    assetGenerateTargetDebugConfiguration->name = "Debug";
                    assetGenerateTargetConfigurationList->configurations.push_back(assetGenerateTargetDebugConfiguration);

                    auto assetGenerateTargetReleaseConfiguration = alloc<XCBuildConfiguration>();
                    assetGenerateTargetReleaseConfiguration->name = "Release";
                    assetGenerateTargetConfigurationList->configurations.push_back(assetGenerateTargetReleaseConfiguration);
                    assetGenerateTargetConfigurationList->defaultConfigurationName = assetGenerateTargetReleaseConfiguration->name;

                    auto assetGenerateTargetProxy = alloc<PBXContainerItemProxy>();
                    assetGenerateTargetProxy->proxyType = PBXContainerItemProxy::ProxyType::nativeTarget;
                    assetGenerateTargetProxy->containerPortal = pbxProject;
                    assetGenerateTargetProxy->remoteGlobal = assetGenerateTarget;
                    assetGenerateTargetProxy->remoteInfo = assetGenerateTarget->name;

                    auto assetGenerateTargetDependency = alloc<PBXTargetDependency>();
                    assetGenerateTargetDependency->targetProxy = assetGenerateTargetProxy;
                    assetGenerateTargetDependency->target = assetGenerateTarget;
                    nativeTarget->dependencies.push_back(assetGenerateTargetDependency);
                }
        }

        void save(const storage::Path& path) const
        {
            const auto projectFileType = storage::FileSystem::getFileType(path);

            if (projectFileType == storage::FileType::notFound)
                storage::FileSystem::createDirectory(path);
            else if (projectFileType != storage::FileType::directory)
            {
                storage::FileSystem::deleteFile(path);
                storage::FileSystem::createDirectory(path);
            }

            const auto pbxProjectPath = path / storage::Path{"project.pbxproj"};
            std::ofstream projectFile(pbxProjectPath, std::ios::trunc);
            projectFile << plist::encode(encode(), plist::Format::ascii, true);

            const auto workspacePath = path / "project.xcworkspace";

            const auto workspaceFileType = storage::FileSystem::getFileType(workspacePath);

            if (workspaceFileType == storage::FileType::notFound)
                storage::FileSystem::createDirectory(workspacePath);
            else if (workspaceFileType != storage::FileType::directory)
            {
                storage::FileSystem::deleteFile(workspacePath);
                storage::FileSystem::createDirectory(workspacePath);
            }

            const auto xcworkspacedataPath = workspacePath / storage::Path{"contents.xcworkspacedata"};
            std::ofstream workspaceFile(xcworkspacedataPath, std::ios::trunc);

            workspaceFile <<
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<Workspace\n"
                "   version = \"1.0\">\n"
                "   <FileRef\n"
                "      location = \"self:" << std::string(path.getFilename()) << "\">\n"
                "   </FileRef>\n"
                "</Workspace>\n";
        }

    private:
        template <class T>
        T* alloc()
        {
            std::unique_ptr<T> object = std::make_unique<T>();
            T* result = object.get();
            objects.push_back(std::move(object));
            return result;
        }

        plist::Value encode() const
        {
            plist::Value result = plist::Dictionary{
                {"archiveVersion", 1},
                {"classes", plist::Dictionary{}},
                {"objectVersion", 51},
                {"objects", plist::Dictionary{}},
                {"rootObject", rootObject ? toString(rootObject->getId()): ""}
            };

            for (const auto& object : objects)
                result["objects"][toString(object->getId())] = object->encode();

            return result;
        }

        static std::string escapeForShell(const std::string& s)
        {
            std::string result;
            result.reserve(s.length());

            for (const auto c : s)
                if ((c >= 'A' && c <= 'Z') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= '0' && c <= '9') ||
                    c == '%' || c == '+' || c == '-' ||
                    c == '.' || c == '/' || c == ':' ||
                    c == '=' || c == '@' || c == '_')
                    result.push_back(c);
                else if (c == ' ' || c == '!' || c == '"' || c == '#' ||
                         c == '$' || c == '&' || c == '\'' || c == '(' ||
                         c == ')' || c == '*' || c == ',' || c == ';' ||
                         c == '<' || c == '>' || c == '?' || c == '[' ||
                         c == '\\' || c == ']' || c == '^' || c == '`' ||
                         c == '{' || c == '|' || c == '}' || c == '~')
                {
                    result.push_back('\\');
                    result.push_back(c);
                }
                else if (c == '\a') result += "$'\\a'";
                else if (c == '\b') result += "$'\\b'";
                else if (c == '\t') result += "$'\\t'";
                else if (c == '\n') result += "$'\\n'";
                else if (c == '\v') result += "$'\\v'";
                else if (c == '\f') result += "$'\\f'";
                else if (c == '\r') result += "$'\\r'";
                else if (c != '\0')
                    result += "$'\\" + std::string{
                        static_cast<char>('0' + ((c >> 6) & 0x07)),
                        static_cast<char>('0' + ((c >> 3) & 0x07)),
                        static_cast<char>('0' + ((c >> 0) & 0x07))
                    } + "'";

            return result;
        }

        std::vector<std::unique_ptr<PBXObject>> objects;
        const PBXObject* rootObject = nullptr;
    };
}

#endif // OUZEL_XCODE_XCODEPROJECT_HPP
