// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../core/Setup.h"

#include <algorithm>
#include <fstream>
#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#  include <ShlObj.h>
#  include <Shlwapi.h>
#  include <strsafe.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__APPLE__)
#  include <TargetConditionals.h>
#  include <objc/message.h>
#  include <objc/NSObjCRuntime.h>
#  include <CoreFoundation/CoreFoundation.h>
#elif defined(__ANDROID__)
#  include "../core/android/EngineAndroid.hpp"
#elif defined(__linux__)
#  include <pwd.h>
#endif

#include "FileSystem.hpp"
#include "Archive.hpp"
#include "../core/Engine.hpp"
#include "../utils/Log.hpp"

#if defined(__APPLE__)
#  include "CfPointer.hpp"
#endif

namespace ouzel
{
    namespace storage
    {
        FileSystem::FileSystem(Engine& initEngine):
            engine(initEngine)
        {
#if defined(_WIN32)
            HINSTANCE instance = GetModuleHandleW(nullptr);
            if (!instance)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");
            std::vector<WCHAR> buffer(MAX_PATH + 1);
            for (;;)
            {
                if (!GetModuleFileNameW(instance, buffer.data(), static_cast<DWORD>(buffer.size())))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get module filename");

                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                    buffer.resize(buffer.size() * 2);
                else
                    break;
            }

            const auto executablePath = Path{buffer.data(), Path::Format::native};
            appPath = executablePath.getDirectory();
            engine.log(Log::Level::info) << "Application directory: " << appPath;

#elif defined(__APPLE__)
            CFBundleRef bundle = CFBundleGetMainBundle();
            if (!bundle)
                throw std::runtime_error("Failed to get main bundle");

            CfPointer<CFURLRef> relativePath = CFBundleCopyResourcesDirectoryURL(bundle);
            if (!relativePath)
                throw std::runtime_error("Failed to get resource directory");

            CfPointer<CFURLRef> absolutePath = CFURLCopyAbsoluteURL(relativePath.get());
            CfPointer<CFStringRef> path = CFURLCopyFileSystemPath(absolutePath.get(), kCFURLPOSIXPathStyle);

            const CFIndex maximumSize = CFStringGetMaximumSizeOfFileSystemRepresentation(path.get());
            std::vector<char> resourceDirectory(static_cast<std::size_t>(maximumSize));
            const Boolean result = CFStringGetFileSystemRepresentation(path.get(), resourceDirectory.data(), maximumSize);
            if (!result)
                throw std::runtime_error("Failed to get resource directory");

            appPath = Path{resourceDirectory.data(), Path::Format::native};
            engine.log(Log::Level::info) << "Application directory: " << appPath;

#elif defined(__ANDROID__)
            // not available for Android

#elif defined(__linux__)
            char executableDirectory[PATH_MAX];

            ssize_t length;
            if ((length = readlink("/proc/self/exe", executableDirectory, sizeof(executableDirectory) - 1)) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to get current directory");

            executableDirectory[length] = '\0';
            const auto executablePath = Path{executableDirectory, Path::Format::native};
            appPath = executablePath.getDirectory();
            engine.log(Log::Level::info) << "Application directory: " << appPath;
#endif
        }

        Path FileSystem::getStorageDirectory(const bool user) const
        {
#if defined(_WIN32)
            WCHAR appDataPath[MAX_PATH];

            HRESULT hr;
            if (FAILED(hr = SHGetFolderPathW(nullptr, (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, appDataPath)))
                throw std::system_error(hr, std::system_category(), "Failed to get the path of the AppData directory");

            Path path = Path{appDataPath, Path::Format::native};

            HINSTANCE instance = GetModuleHandleW(nullptr);
            if (!instance)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");
            std::vector<WCHAR> buffer(MAX_PATH + 1);
            for (;;)
            {
                if (!GetModuleFileNameW(instance, buffer.data(), static_cast<DWORD>(buffer.size())))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get module filename");

                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                    buffer.resize(buffer.size() * 2);
                else
                    break;
            }

            const auto executablePath = Path{buffer.data(), Path::Format::native};
            DWORD handle;
            const DWORD fileVersionSize = GetFileVersionInfoSizeW(executablePath.getNative().c_str(), &handle);
            if (!fileVersionSize)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get file version size");

            auto fileVersionBuffer = std::make_unique<char[]>(fileVersionSize);
            if (!GetFileVersionInfoW(executablePath.getNative().c_str(),
                                     handle, fileVersionSize,
                                     fileVersionBuffer.get()))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get file version");

            LPWSTR companyName = nullptr;
            LPWSTR productName = nullptr;

            struct LANGANDCODEPAGE final
            {
                WORD wLanguage;
                WORD wCodePage;
            };
            LPVOID translationPointer;
            UINT translationLength;

            if (VerQueryValueW(fileVersionBuffer.get(),
                               L"\\VarFileInfo\\Translation",
                               &translationPointer,
                               &translationLength))
            {
                auto translation = static_cast<const LANGANDCODEPAGE*>(translationPointer);

                for (UINT i = 0; i < translationLength / sizeof(LANGANDCODEPAGE); ++i)
                {
                    constexpr size_t subBlockSize = 37;
                    WCHAR subBlock[subBlockSize];

                    StringCchPrintfW(subBlock, subBlockSize,
                                     L"\\StringFileInfo\\040904b0\\CompanyName",
                                     translation[i].wLanguage,
                                     translation[i].wCodePage);

                    LPVOID companyNamePointer;
                    UINT companyNameLength;
                    if (VerQueryValueW(fileVersionBuffer.get(),
                                       subBlock,
                                       &companyNamePointer,
                                       &companyNameLength))
                        companyName = static_cast<LPWSTR>(companyNamePointer);


                    StringCchPrintfW(subBlock, subBlockSize,
                                     L"\\StringFileInfo\\040904b0\\ProductName",
                                     translation[i].wLanguage,
                                     translation[i].wCodePage);

                    LPVOID productNamePointer;
                    UINT productNameLength;
                    if (VerQueryValueW(fileVersionBuffer.get(),
                                       subBlock,
                                       &productNamePointer,
                                       &productNameLength))
                        productName = static_cast<LPWSTR>(productNamePointer);
                }
            }

            if (companyName)
                path /= companyName;
            else
                path /= OUZEL_DEVELOPER_NAME;

            if (getFileType(path) != FileType::directory)
                createDirectory(path);

            if (productName)
                path /= productName;
            else
                path /= OUZEL_APPLICATION_NAME;

            if (getFileType(path) != FileType::directory)
                createDirectory(path);

            return path;
#elif TARGET_OS_IOS || TARGET_OS_TV
            id fileManager = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("NSFileManager"), sel_getUid("defaultManager"));

            constexpr NSUInteger NSDocumentDirectory = 9;
            constexpr NSUInteger NSUserDomainMask = 1;
            constexpr NSUInteger NSLocalDomainMask = 2;

            id documentDirectory = reinterpret_cast<id (*)(id, SEL, NSUInteger, NSUInteger, id, BOOL, id*)>(&objc_msgSend)(fileManager, sel_getUid("URLForDirectory:inDomain:appropriateForURL:create:error:"), NSDocumentDirectory, user ? NSUserDomainMask : NSLocalDomainMask, nil, YES, nil);

            if (!documentDirectory)
                throw std::runtime_error("Failed to get document directory");

            id documentDirectoryString = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(documentDirectory, sel_getUid("path"));
            auto pathUtf8String = reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(documentDirectoryString, sel_getUid("UTF8String"));
            return Path{pathUtf8String, Path::Format::native};
#elif TARGET_OS_MAC
            id fileManager = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("NSFileManager"), sel_getUid("defaultManager"));

            constexpr NSUInteger NSApplicationSupportDirectory = 14;
            constexpr NSUInteger NSUserDomainMask = 1;
            constexpr NSUInteger NSLocalDomainMask = 2;

            id applicationSupportDirectory = reinterpret_cast<id (*)(id, SEL, NSUInteger, NSUInteger, id, BOOL, id*)>(&objc_msgSend)(fileManager, sel_getUid("URLForDirectory:inDomain:appropriateForURL:create:error:"), NSApplicationSupportDirectory, user ? NSUserDomainMask : NSLocalDomainMask, nil, YES, nil);

            if (!applicationSupportDirectory)
                throw std::runtime_error("Failed to get application support directory");

            CFBundleRef bundle = CFBundleGetMainBundle();
            CFStringRef identifier = CFBundleGetIdentifier(bundle);

            if (!identifier)
                identifier = CFSTR(OUZEL_DEVELOPER_NAME "." OUZEL_APPLICATION_NAME);

            id path = reinterpret_cast<id (*)(id, SEL, CFStringRef)>(&objc_msgSend)(applicationSupportDirectory, sel_getUid("URLByAppendingPathComponent:"), identifier);
            reinterpret_cast<void (*)(id, SEL, id, BOOL, id, id)>(&objc_msgSend)(fileManager, sel_getUid("createDirectoryAtURL:withIntermediateDirectories:attributes:error:"), path, YES, nil, nil);
            id pathString = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(path, sel_getUid("path"));
            auto pathUtf8String = reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(pathString, sel_getUid("UTF8String"));
            return Path{pathUtf8String, Path::Format::native};
#elif defined(__ANDROID__)
            static_cast<void>(user);

            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);
            return engineAndroid.getFilesDirectory();
#elif defined(__linux__)
            Path path;

            const char* homeDirectory = std::getenv("XDG_DATA_HOME");

            if (homeDirectory)
                path = Path{homeDirectory, Path::Format::native};
            else
            {
                struct passwd pwent;
                struct passwd* pwentp;
                std::vector<char> buffer(1024);
                int e;

                while ((e = getpwuid_r(getuid(), &pwent, buffer.data(), buffer.size(), &pwentp)) == ERANGE)
                    buffer.resize(buffer.size() * 2);

                if (e != 0)
                    throw std::runtime_error("Failed to get home directory");
                else
                    path = Path{pwent.pw_dir, Path::Format::native};

                path /= ".local";

                if (getFileType(path) != FileType::directory)
                    createDirectory(path);

                path /= "share";

                if (getFileType(path) != FileType::directory)
                    createDirectory(path);
            }

            path /= OUZEL_DEVELOPER_NAME;

            if (getFileType(path) != FileType::directory)
                createDirectory(path);

            path /= OUZEL_APPLICATION_NAME;

            if (getFileType(path) != FileType::directory)
                createDirectory(path);

            return path;
#else
            return Path{};
#endif
        }

        std::vector<std::uint8_t> FileSystem::readFile(const Path& filename, const bool searchResources)
        {
            if (searchResources)
                for (auto& archive : archives)
                    if (archive.second.fileExists(filename))
                        return archive.second.readFile(filename);

#if defined(__ANDROID__)
            if (!filename.isAbsolute())
            {
                EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

                AAsset* asset = AAssetManager_open(engineAndroid.getAssetManager(), filename.getNative().c_str(), AASSET_MODE_STREAMING);

                if (!asset)
                    throw std::runtime_error("Failed to open file " + std::string(filename));

                std::vector<std::uint8_t> data;
                std::uint8_t buffer[1024];

                for (;;)
                {
                    const int bytesRead = AAsset_read(asset, buffer, sizeof(buffer));

                    if (bytesRead < 0)
                        throw std::runtime_error("Failed to read from file");
                    else if (bytesRead == 0)
                        break;

                    data.insert(data.end(), buffer, buffer + bytesRead);
                }

                AAsset_close(asset);

                return data;
            }
#endif

            const auto path = getPath(filename, searchResources);

            // file does not exist
            if (path.isEmpty())
                throw std::runtime_error("Failed to find file " + std::string(filename));

            std::ifstream f(path, std::ios::binary);
            return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        }

        bool FileSystem::resourceFileExists(const Path& filename) const
        {
            if (filename.isAbsolute())
                return fileExists(filename);
            else
            {
                Path result = appPath / filename;

                if (fileExists(result))
                    return true;
                else
                    for (const auto& path : resourcePaths)
                    {
                        if (path.isAbsolute()) // if resource path is absolute
                            result = path / filename;
                        else
                            result = appPath / path / filename;

                        if (fileExists(result))
                            return true;
                    }

                return false;
            }
        }

        bool FileSystem::directoryExists(const Path& dirname) const
        {
#if defined(__ANDROID__)
            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

            AAssetDir* assetDir = AAssetManager_openDir(engineAndroid.getAssetManager(), dirname.getNative().c_str());
            const bool exists = AAssetDir_getNextFileName(assetDir) != nullptr;
            AAssetDir_close(assetDir);

            if (exists) return true;
#endif

            return getFileType(dirname) == FileType::directory;
        }

        bool FileSystem::fileExists(const Path& filename) const
        {
#if defined(__ANDROID__)
            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

            AAsset* asset = AAssetManager_open(engineAndroid.getAssetManager(), filename.getNative().c_str(), AASSET_MODE_STREAMING);

            if (asset)
            {
                AAsset_close(asset);
                return true;
            }
#endif

            return getFileType(filename) == FileType::regular;
        }
    } // namespace storage
} // namespace ouzel
