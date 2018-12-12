// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#include <algorithm>
#include <stdexcept>
#include <system_error>
#if OUZEL_PLATFORM_WINDOWS
#  include <Windows.h>
#  include <Shlobj.h>
#  include <Shlwapi.h>
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#  include <objc/message.h>
#  include <objc/NSObjCRuntime.h>
#  include <CoreFoundation/CoreFoundation.h>
extern "C" id NSTemporaryDirectory();
#elif OUZEL_PLATFORM_LINUX
#  include <pwd.h>
#  include <unistd.h>
#elif OUZEL_PLATFORM_ANDROID
#  include "core/android/EngineAndroid.hpp"
#endif

#if !OUZEL_PLATFORM_WINDOWS
#  include <sys/stat.h>
#endif

#include "FileSystem.hpp"
#include "File.hpp"
#include "Archive.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
#if OUZEL_PLATFORM_WINDOWS
    const std::string FileSystem::DIRECTORY_SEPARATOR = "\\";
#else
    const std::string FileSystem::DIRECTORY_SEPARATOR = "/";
#endif

    FileSystem::FileSystem(Engine& initEngine):
        engine(initEngine)
    {
#if OUZEL_PLATFORM_WINDOWS
        std::vector<WCHAR> buffer(MAX_PATH);
        for (;;)
        {
            HINSTANCE instance = GetModuleHandleW(nullptr);
            if (!instance)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

            if (!GetModuleFileNameW(instance, buffer.data(), static_cast<DWORD>(buffer.size())))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get module filename");

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                buffer.resize(buffer.size() * 2);
            else
                break;
        }

        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, nullptr, 0, nullptr, nullptr);
        if (bufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

        std::vector<char> appFilename(bufferSize);
        if (WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, appFilename.data(), bufferSize, nullptr, nullptr) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

        appPath = getDirectoryPart(appFilename.data());
        engine.log(Log::Level::INFO) << "Application directory: " << appPath;

#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        CFBundleRef bundle = CFBundleGetMainBundle();
        CFURLRef path = CFBundleCopyResourcesDirectoryURL(bundle);

        if (!path)
            throw std::runtime_error("Failed to get current directory");

        char resourceDirectory[1024];
        CFURLGetFileSystemRepresentation(path, TRUE, reinterpret_cast<UInt8*>(resourceDirectory), sizeof(resourceDirectory));
        CFRelease(path);
        appPath = resourceDirectory;
        engine.log(Log::Level::INFO) << "Application directory: " << appPath;

#elif OUZEL_PLATFORM_LINUX
        char executableDirectory[1024];

        ssize_t length;
        if ((length = readlink("/proc/self/exe", executableDirectory, sizeof(executableDirectory))) == -1)
            throw std::system_error(errno, std::system_category(), "Failed to get current directory");

        executableDirectory[length] = '\0';
        appPath = getDirectoryPart(executableDirectory);
        engine.log(Log::Level::INFO) << "Application directory: " << appPath;
#endif
    }

    std::string FileSystem::getStorageDirectory(bool user) const
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR appDataPath[MAX_PATH];

        HRESULT hr;
        if (FAILED(hr = SHGetFolderPathW(nullptr, (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, appDataPath)))
            throw std::system_error(hr, std::system_category(), "Failed to get the path of the AppData directory");

        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, nullptr, 0, nullptr, nullptr);
        if (bufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

        std::vector<char> appDataBuffer(bufferSize);
        if (WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, appDataBuffer.data(), bufferSize, nullptr, nullptr) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

        std::string path = appDataBuffer.data();

        path += DIRECTORY_SEPARATOR + OUZEL_DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            bufferSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
            if (bufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> buffer(bufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer.data(), bufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            // relative paths longer than MAX_PATH are not supported
            if (buffer.size() > MAX_PATH)
                buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

            DWORD attributes = GetFileAttributesW(buffer.data());
            if (attributes == INVALID_FILE_ATTRIBUTES)
            {
                if (!CreateDirectoryW(buffer.data(), nullptr))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create directory " + path);
            }
            else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                throw std::runtime_error(path + " is not a directory");
        }

        path += DIRECTORY_SEPARATOR + OUZEL_APPLICATION_NAME;

        if (!directoryExists(path))
        {
            bufferSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
            if (bufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> buffer(bufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer.data(), bufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            // relative paths longer than MAX_PATH are not supported
            if (buffer.size() > MAX_PATH)
                buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

            DWORD attributes = GetFileAttributesW(buffer.data());
            if (attributes == INVALID_FILE_ATTRIBUTES)
            {
                if (!CreateDirectoryW(buffer.data(), nullptr))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create directory " + path);
            }
            else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                throw std::runtime_error(path + " is not a directory");
        }

        return path;
#elif OUZEL_PLATFORM_MACOS
        id fileManager = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("NSFileManager"), sel_getUid("defaultManager"));

        static constexpr NSUInteger NSApplicationSupportDirectory = 14;
        static constexpr NSUInteger NSUserDomainMask = 1;
        static constexpr NSUInteger NSLocalDomainMask = 2;

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
        return reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(pathString, sel_getUid("UTF8String"));
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        id fileManager = reinterpret_cast<id (*)(Class, SEL)>(&objc_msgSend)(objc_getClass("NSFileManager"), sel_getUid("defaultManager"));

        static constexpr NSUInteger NSDocumentDirectory = 9;
        static constexpr NSUInteger NSUserDomainMask = 1;
        static constexpr NSUInteger NSLocalDomainMask = 2;

        id documentDirectory = reinterpret_cast<id (*)(id, SEL, NSUInteger, NSUInteger, id, BOOL, id*)>(&objc_msgSend)(fileManager, sel_getUid("URLForDirectory:inDomain:appropriateForURL:create:error:"), NSDocumentDirectory, user ? NSUserDomainMask : NSLocalDomainMask, nil, YES, nil);

        if (!documentDirectory)
            throw std::runtime_error("Failed to get document directory");

        id documentDirectoryString = reinterpret_cast<id (*)(id, SEL)>(&objc_msgSend)(documentDirectory, sel_getUid("path"));
        return reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(documentDirectoryString, sel_getUid("UTF8String"));
#elif OUZEL_PLATFORM_LINUX
        std::string path;

        char* homeDirectory = getenv("XDG_DATA_HOME");

        if (homeDirectory)
            path = homeDirectory;
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
                path = pwent.pw_dir;

            path += DIRECTORY_SEPARATOR + ".local";

            if (!directoryExists(path))
                if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);

            path += DIRECTORY_SEPARATOR + "share";

            if (!directoryExists(path))
                if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);
        }

        path += DIRECTORY_SEPARATOR + OUZEL_DEVELOPER_NAME;

        if (!directoryExists(path))
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);

        path += DIRECTORY_SEPARATOR + OUZEL_APPLICATION_NAME;

        if (!directoryExists(path))
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);

        return path;
#elif OUZEL_PLATFORM_ANDROID
        (void)user;

        EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);
        return engineAndroid.getFilesDirectory();
#else
        return "";
#endif
    }

    std::string FileSystem::getTempDirectory() const
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR buffer[MAX_PATH];
        if (GetTempPathW(MAX_PATH, buffer))
        {
            int bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
            if (bufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

            std::vector<char> tempDirectoryBuffer(bufferSize);
            if (WideCharToMultiByte(CP_UTF8, 0, buffer, -1, tempDirectoryBuffer.data(), bufferSize, nullptr, nullptr) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

            return tempDirectoryBuffer.data();
        }
        else
            return "";
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        id temporaryDirectory = NSTemporaryDirectory();
        return reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(temporaryDirectory, sel_getUid("UTF8String")); // [temporaryDirectory UTF8String]
#elif OUZEL_PLATFORM_LINUX
        char const* path = getenv("TMPDIR");
        if (path)
            return path;
        else
            return "/tmp";
#elif OUZEL_PLATFORM_ANDROID
        EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);
        return engineAndroid.getCacheDirectory();
#else
        return "";
#endif
    }

    std::vector<uint8_t> FileSystem::readFile(const std::string& filename, bool searchResources) const
    {
        if (searchResources)
        {
            for (const auto& archive : archives)
            {
                if (archive->fileExists(filename))
                    return archive->readFile(filename);
            }
        }

        std::vector<uint8_t> data;
        char buffer[1024];

#if OUZEL_PLATFORM_ANDROID
        if (pathIsRelative(filename))
        {
            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

            AAsset* asset = AAssetManager_open(engineAndroid.getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

            if (!asset)
                throw std::runtime_error("Failed to open file " + filename);

            int bytesRead = 0;

            while ((bytesRead = AAsset_read(asset, buffer, sizeof(buffer))) > 0)
                data.insert(data.end(), reinterpret_cast<uint8_t*>(buffer), reinterpret_cast<uint8_t*>(buffer + bytesRead));

            AAsset_close(asset);

            return data;
        }
#endif

        std::string path = getPath(filename, searchResources);

        // file does not exist
        if (path.empty())
            throw std::runtime_error("Failed to find file " + filename);

        File file(path, File::Mode::READ);

        while (uint32_t size = file.read(buffer, sizeof(buffer)))
            data.insert(data.end(), buffer, buffer + size);

        return data;
    }

    void FileSystem::writeFile(const std::string& filename, const std::vector<uint8_t>& data) const
    {
        File file(filename, File::Mode::WRITE | File::Mode::CREATE | File::Mode::TRUNCATE);

        uint32_t offset = 0;

        while (offset < data.size())
        {
            uint32_t written = file.write(data.data() + offset, static_cast<uint32_t>(data.size()) - offset);
            offset += written;
        }
    }

    bool FileSystem::resourceFileExists(const std::string& filename) const
    {
        if (!pathIsRelative(filename))
            return fileExists(filename);
        else
        {
            std::string str = appPath + DIRECTORY_SEPARATOR + filename;

            if (fileExists(str))
                return true;
            else
            {
                for (const std::string& path : resourcePaths)
                {
                    if (!pathIsRelative(path)) // if resource path is absolute
                        str = path + DIRECTORY_SEPARATOR + filename;
                    else
                        str = appPath + DIRECTORY_SEPARATOR + path + DIRECTORY_SEPARATOR + filename;

                    if (fileExists(str))
                        return true;
                }
            }

            return false;
        }
    }

    bool FileSystem::directoryExists(const std::string& dirname) const
    {
#if OUZEL_PLATFORM_ANDROID
        EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

        AAssetDir* assetDir = AAssetManager_openDir(engineAndroid.getAssetManager(), dirname.c_str());
        bool exists = AAssetDir_getNextFileName(assetDir) != nullptr;
        AAssetDir_close(assetDir);

        if (exists) return true;
#endif

#if OUZEL_PLATFORM_WINDOWS
        int bufferSize = MultiByteToWideChar(CP_UTF8, 0, dirname.c_str(), -1, nullptr, 0);
        if (bufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> buffer(bufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, dirname.c_str(), -1, buffer.data(), bufferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        // relative paths longer than MAX_PATH are not supported
        if (buffer.size() > MAX_PATH)
            buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

        DWORD attributes = GetFileAttributesW(buffer.data());
        if (attributes == INVALID_FILE_ATTRIBUTES)
            return false;

        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
        struct stat buf;
        if (stat(dirname.c_str(), &buf) == -1)
            return false;

        return (buf.st_mode & S_IFMT) == S_IFDIR;
#endif
    }

    bool FileSystem::fileExists(const std::string& filename) const
    {
#if OUZEL_PLATFORM_ANDROID
        EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

        AAsset* asset = AAssetManager_open(engineAndroid.getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

        if (asset)
        {
            AAsset_close(asset);
            return true;
        }
#endif

#if OUZEL_PLATFORM_WINDOWS
        int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0);
        if (bufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> buffer(bufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer.data(), bufferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        // relative paths longer than MAX_PATH are not supported
        if (buffer.size() > MAX_PATH)
            buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

        DWORD attributes = GetFileAttributesW(buffer.data());
        if (attributes == INVALID_FILE_ATTRIBUTES)
            return false;

        return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
        struct stat buf;
        if (stat(filename.c_str(), &buf) == -1)
            return false;

        return (buf.st_mode & S_IFMT) == S_IFREG;
#endif
    }

    std::string FileSystem::getPath(const std::string& filename, bool searchResources) const
    {
        if (!pathIsRelative(filename))
        {
            if (fileExists(filename))
                return filename;
        }
        else
        {
            std::string str = appPath + DIRECTORY_SEPARATOR + filename;

            if (fileExists(str))
                return str;

            if (searchResources)
            {
                for (const std::string& path : resourcePaths)
                {
                    if (!pathIsRelative(path)) // if resource path is absolute
                        str = path + DIRECTORY_SEPARATOR + filename;
                    else
                        str = appPath + DIRECTORY_SEPARATOR + path + DIRECTORY_SEPARATOR + filename;

                    if (fileExists(str))
                        return str;
                }
            }
        }

        return "";
    }

    void FileSystem::addResourcePath(const std::string& path)
    {
        auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

        if (i == resourcePaths.end())
            resourcePaths.push_back(path);
    }

    void FileSystem::addArchive(Archive* archive)
    {
        auto i = std::find(archives.begin(), archives.end(), archive);

        if (i == archives.end())
            archives.push_back(archive);
    }

    void FileSystem::removeArchive(Archive* archive)
    {
        auto i = std::find(archives.begin(), archives.end(), archive);

        if (i != archives.end())
            archives.erase(i);
    }

    std::string FileSystem::getExtensionPart(const std::string& path)
    {
        size_t pos = path.find_last_of('.');

        if (pos != std::string::npos)
            return path.substr(pos + 1);

        return std::string();
    }

    std::string FileSystem::getFilenamePart(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");

        if (pos != std::string::npos)
            return path.substr(pos + 1);
        else
            return path;
    }

    std::string FileSystem::getDirectoryPart(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");

        if (pos != std::string::npos)
            return path.substr(0, pos);

        return std::string();
    }

    bool FileSystem::pathIsRelative(const std::string& path)
    {
#if OUZEL_PLATFORM_WINDOWS
        int bufferSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
        if (bufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> buffer(bufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer.data(), bufferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        // relative paths longer than MAX_PATH are not supported
        if (buffer.size() > MAX_PATH)
            buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

        return PathIsRelativeW(buffer.data()) == TRUE;
#else
        return path.empty() || path[0] != '/';
#endif
    }
}
