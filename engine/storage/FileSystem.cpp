// Ouzel by Elviss Strazdins

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

#ifdef __APPLE__
#  include "../platform/corefoundation/Pointer.hpp"
#endif

namespace ouzel::storage
{
    FileSystem::FileSystem(core::Engine& initEngine):
        engine{initEngine}
    {
#if defined(_WIN32)
        HINSTANCE instance = GetModuleHandleW(nullptr);
        if (!instance)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get module handle"};
        std::vector<WCHAR> buffer(MAX_PATH + 1);
        for (;;)
        {
            if (!GetModuleFileNameW(instance, buffer.data(), static_cast<DWORD>(buffer.size())))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get module filename"};

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                buffer.resize(buffer.size() * 2);
            else
                break;
        }

        const auto executablePath = Path{buffer.data(), Path::Format::native};
        appPath = executablePath.getDirectory();
        log(Log::Level::info) << "Application directory: " << appPath;

#elif defined(__APPLE__)
        CFBundleRef bundle = CFBundleGetMainBundle();
        if (!bundle)
            throw std::runtime_error{"Failed to get main bundle"};

        const platform::corefoundation::Pointer relativePath = CFBundleCopyResourcesDirectoryURL(bundle);
        if (!relativePath)
            throw std::runtime_error{"Failed to get resource directory"};

        const platform::corefoundation::Pointer absolutePath = CFURLCopyAbsoluteURL(relativePath.get());
        if (!absolutePath)
            throw std::runtime_error{"Failed to copy absolute URL"};

        const platform::corefoundation::Pointer path = CFURLCopyFileSystemPath(absolutePath.get(), kCFURLPOSIXPathStyle);
        if (!path)
            throw std::runtime_error{"Failed to copy file system path"};

        const auto maximumSize = CFStringGetMaximumSizeOfFileSystemRepresentation(path.get());
        const auto resourceDirectory = std::make_unique<char[]>(static_cast<std::size_t>(maximumSize));
        if (!CFStringGetFileSystemRepresentation(path.get(), resourceDirectory.get(), maximumSize))
            throw std::runtime_error{"Failed to get resource directory"};

        appPath = Path{resourceDirectory.get(), Path::Format::native};
        log(Log::Level::info) << "Application directory: " << appPath;

#elif defined(__ANDROID__)
        // not available for Android

#elif defined(__linux__)
        char executableDirectory[PATH_MAX];

        const auto length = readlink("/proc/self/exe", executableDirectory, sizeof(executableDirectory) - 1);
        if (length == -1)
            throw std::system_error{errno, std::system_category(), "Failed to get current directory"};

        executableDirectory[length] = '\0';
        const auto executablePath = Path{executableDirectory, Path::Format::native};
        appPath = executablePath.getDirectory();
        log(Log::Level::info) << "Application directory: " << appPath;
#else
#  error "Unsupported platform"
#endif
    }

    Path FileSystem::getStorageDirectory([[maybe_unused]] const bool user) const
    {
#if defined(_WIN32)
        WCHAR appDataPath[MAX_PATH];

        const auto folderId = (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE;
        if (const auto hr = SHGetFolderPathW(nullptr,
                                             folderId,
                                             nullptr,
                                             SHGFP_TYPE_CURRENT,
                                             appDataPath); FAILED(hr))
            throw std::system_error{hr, std::system_category(), "Failed to get the path of the AppData directory"};

        auto path = Path{appDataPath, Path::Format::native};

        const auto instance = GetModuleHandleW(nullptr);
        if (!instance)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get module handle"};
        std::vector<WCHAR> buffer(MAX_PATH + 1);
        for (;;)
        {
            if (!GetModuleFileNameW(instance, buffer.data(), static_cast<DWORD>(buffer.size())))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get module filename"};

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                buffer.resize(buffer.size() * 2);
            else
                break;
        }

        const auto executablePath = Path{buffer.data(), Path::Format::native};
        DWORD handle;
        const auto fileVersionSize = GetFileVersionInfoSizeW(executablePath.getNative().c_str(), &handle);
        if (!fileVersionSize)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get file version size"};

        auto fileVersionBuffer = std::make_unique<char[]>(fileVersionSize);
        if (!GetFileVersionInfoW(executablePath.getNative().c_str(),
                                 0, fileVersionSize,
                                 fileVersionBuffer.get()))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get file version"};

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
            const auto translation = static_cast<const LANGANDCODEPAGE*>(translationPointer);

            for (UINT i = 0; i < translationLength / sizeof(LANGANDCODEPAGE); ++i)
            {
                constexpr std::size_t subBlockSize = 37U;
                WCHAR subBlock[subBlockSize];

                StringCchPrintfW(subBlock, subBlockSize,
                                 L"\\StringFileInfo\\%04x%04x\\CompanyName",
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
                                 L"\\StringFileInfo\\%04x%04x\\ProductName",
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
        const auto fileManager = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("NSFileManager"), sel_getUid("defaultManager"));

        constexpr NSUInteger NSDocumentDirectory = 9U;
        constexpr NSUInteger NSUserDomainMask = 1U;
        constexpr NSUInteger NSLocalDomainMask = 2U;

        const auto documentDirectory = reinterpret_cast<id (*)(id, SEL, NSUInteger, NSUInteger, id, BOOL, id*)>(&objc_msgSend)(fileManager, sel_getUid("URLForDirectory:inDomain:appropriateForURL:create:error:"), NSDocumentDirectory, user ? NSUserDomainMask : NSLocalDomainMask, nil, YES, nil);

        if (!documentDirectory)
            throw std::runtime_error{"Failed to get document directory"};

        const auto documentDirectoryString = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(documentDirectory, sel_getUid("path"));
        const auto pathUtf8String = reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(documentDirectoryString, sel_getUid("UTF8String"));
        return Path{pathUtf8String, Path::Format::native};
#elif TARGET_OS_MAC
        const auto fileManager = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("NSFileManager"), sel_getUid("defaultManager"));

        constexpr NSUInteger NSApplicationSupportDirectory = 14U;
        constexpr NSUInteger NSUserDomainMask = 1U;
        constexpr NSUInteger NSLocalDomainMask = 2U;

        const auto applicationSupportDirectory = reinterpret_cast<id (*)(id, SEL, NSUInteger, NSUInteger, id, BOOL, id*)>(&objc_msgSend)(fileManager, sel_getUid("URLForDirectory:inDomain:appropriateForURL:create:error:"), NSApplicationSupportDirectory, user ? NSUserDomainMask : NSLocalDomainMask, nil, YES, nil);

        if (!applicationSupportDirectory)
            throw std::runtime_error{"Failed to get application support directory"};

        CFBundleRef bundle = CFBundleGetMainBundle();
        if (!bundle)
            throw std::runtime_error{"Failed to get main bundle"};

        CFStringRef identifier = CFBundleGetIdentifier(bundle);
        if (!identifier)
            identifier = CFSTR(OUZEL_DEVELOPER_NAME "." OUZEL_APPLICATION_NAME);

        const auto path = reinterpret_cast<id (*)(id, SEL, CFStringRef)>(&objc_msgSend)(applicationSupportDirectory, sel_getUid("URLByAppendingPathComponent:"), identifier);
        reinterpret_cast<void (*)(id, SEL, id, BOOL, id, id)>(&objc_msgSend)(fileManager, sel_getUid("createDirectoryAtURL:withIntermediateDirectories:attributes:error:"), path, YES, nil, nil);
        const auto pathString = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(path, sel_getUid("path"));
        const auto pathUtf8String = reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(pathString, sel_getUid("UTF8String"));
        return Path{pathUtf8String, Path::Format::native};
#elif defined(__ANDROID__)
        auto& engineAndroid = static_cast<core::android::Engine&>(engine);
        return engineAndroid.getFilesDirectory();
#elif defined(__linux__)
        Path path;

        if (const auto homeDirectory = std::getenv("XDG_DATA_HOME"))
            path = Path{homeDirectory, Path::Format::native};
        else
        {
            struct passwd pwent;
            struct passwd* pwentp;
            std::vector<char> buffer(1024);
            int result;

            while ((result = getpwuid_r(getuid(), &pwent, buffer.data(), buffer.size(), &pwentp)) != 0)
                if (result == ERANGE)
                    buffer.resize(buffer.size() * 2);
                else
                    throw std::system_error{result, std::system_category(), "Failed to get password record"};

            if (!pwentp)
                throw std::system_error{result, std::system_category(), "No matching password record found"};

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
#  error "Unsupported platform"
#endif
    }

    std::vector<std::byte> FileSystem::readFile(const Path& filename, const bool searchResources)
    {
        if (searchResources)
        {
            const auto& genericPath = filename.getGeneric();

            for (auto& archive : archives)
                if (archive.second.fileExists(genericPath))
                    return archive.second.readFile(genericPath);
        }

#ifdef __ANDROID__
        if (!filename.isAbsolute())
        {
            auto& engineAndroid = static_cast<core::android::Engine&>(engine);

            const std::unique_ptr<AAsset, decltype(&AAsset_close)> asset{
                AAssetManager_open(engineAndroid.getAssetManager(), filename.getNative().c_str(), AASSET_MODE_STREAMING),
                AAsset_close
            };

            if (!asset)
                throw std::runtime_error{"Failed to open file " + std::string(filename)};

            std::vector<std::byte> data;
            std::byte buffer[1024];

            for (;;)
            {
                const auto bytesRead = AAsset_read(asset.get(), buffer, sizeof(buffer));

                if (bytesRead < 0)
                    throw std::runtime_error{"Failed to read from file"};
                else if (bytesRead == 0)
                    break;

                data.insert(data.end(), buffer, buffer + bytesRead);
            }

            return data;
        }
#endif

        const auto path = getPath(filename, searchResources);

        // file does not exist
        if (path.isEmpty())
            throw std::runtime_error{"Failed to find file " + std::string(filename)};

        std::ifstream file{path, std::ios::binary};
        if (!file)
            throw std::runtime_error{"Failed to open file " + std::string(filename)};

        std::vector<std::byte> data;
        std::byte buffer[1024];

        while (!file.eof())
        {
            file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
            data.insert(data.end(), buffer, buffer + file.gcount());
        }

        return data;
    }

    bool FileSystem::resourceFileExists(const Path& filename) const
    {
        if (filename.isAbsolute())
            return fileExists(filename);
        else
        {
            auto result = appPath / filename;

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
#ifdef __ANDROID__
        auto& engineAndroid = static_cast<core::android::Engine&>(engine);

        auto assetDir = AAssetManager_openDir(engineAndroid.getAssetManager(), dirname.getNative().c_str());
        const auto exists = AAssetDir_getNextFileName(assetDir) != nullptr;
        AAssetDir_close(assetDir);

        if (exists) return true;
#endif
        return getFileType(dirname) == FileType::directory;
    }

    bool FileSystem::fileExists(const Path& filename) const
    {
#ifdef __ANDROID__
        auto& engineAndroid = static_cast<core::android::Engine&>(engine);

        const std::unique_ptr<AAsset, decltype(&AAsset_close)> asset{
            AAssetManager_open(engineAndroid.getAssetManager(), filename.getNative().c_str(), AASSET_MODE_STREAMING),
            AAsset_close
        };

        if (asset) return true;
#endif
        return getFileType(filename) == FileType::regular;
    }
}
