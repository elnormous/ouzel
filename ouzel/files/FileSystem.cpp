// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#include <algorithm>
#include <sys/stat.h>
#if OUZEL_PLATFORM_WINDOWS
#include <Windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <objc/message.h>
#include <CoreFoundation/CoreFoundation.h>
#include "apple/FileSystemApple.hpp"
extern "C" id NSTemporaryDirectory();
#elif OUZEL_PLATFORM_LINUX
#include <pwd.h>
#include <unistd.h>
#elif OUZEL_PLATFORM_ANDROID
#include "core/android/EngineAndroid.hpp"
#endif

#include "FileSystem.hpp"
#include "File.hpp"
#include "Archive.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

#if OUZEL_PLATFORM_WINDOWS || OUZEL_PLATFORM_LINUX
extern std::string DEVELOPER_NAME;
extern std::string APPLICATION_NAME;
#endif

namespace ouzel
{
#if OUZEL_PLATFORM_WINDOWS
    const std::string FileSystem::DIRECTORY_SEPARATOR = "\\";
#else
    const std::string FileSystem::DIRECTORY_SEPARATOR = "/";
#endif

    FileSystem::FileSystem()
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR buffer[MAX_PATH];
        if (!GetModuleFileNameW(GetModuleHandle(nullptr), buffer, MAX_PATH))
            throw FileError("Failed to get module filename");

        char appFilename[1024];
        if (WideCharToMultiByte(CP_UTF8, 0, buffer, -1, appFilename, sizeof(appFilename), nullptr, nullptr) == 0)
            throw FileError("Failed to convert wide char to UTF-8");

        appPath = getDirectoryPart(appFilename);
        Log(Log::Level::INFO) << "Application directory: " << appPath;

#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        CFBundleRef bundle = CFBundleGetMainBundle(); // [NSBundle mainBundle]
        CFURLRef path = CFBundleCopyResourcesDirectoryURL(bundle); // [bundle resourceURL]

        if (path)
        {
            char resourceDirectory[1024];
            CFURLGetFileSystemRepresentation(path, TRUE, reinterpret_cast<UInt8*>(resourceDirectory), sizeof(resourceDirectory));
            CFRelease(path);
            appPath = resourceDirectory;
            Log(Log::Level::INFO) << "Application directory: " << appPath;
        }
        else
            throw FileError("Failed to get current directory");

#elif OUZEL_PLATFORM_LINUX
        char executableDirectory[1024];

        if (readlink("/proc/self/exe", executableDirectory, sizeof(executableDirectory)) != -1)
        {
            appPath = getDirectoryPart(executableDirectory);
            Log(Log::Level::INFO) << "Application directory: " << appPath;
        }
        else
            throw FileError("Failed to get current directory");
#endif
    }

    std::string FileSystem::getStorageDirectory(bool user) const
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR buffer[MAX_PATH];
        char appDataDirectory[1024];

        HRESULT hr = SHGetFolderPathW(nullptr, (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, buffer);
        if (FAILED(hr))
            throw FileError("Failed to get the path of the AppData directory, error: " + std::to_string(hr));

        if (WideCharToMultiByte(CP_UTF8, 0, buffer, -1, appDataDirectory, sizeof(appDataDirectory), nullptr, nullptr) == 0)
            throw FileError("Failed to convert wide char to UTF-8");

        std::string path = appDataDirectory;

        path += DIRECTORY_SEPARATOR + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer, MAX_PATH) == 0)
                throw FileError("Failed to convert UTF-8 to wide char");

            if (!CreateDirectoryW(buffer, nullptr))
                throw FileError("Failed to create directory " + path);
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer, MAX_PATH) == 0)
                throw FileError("Failed to convert UTF-8 to wide char");

            if (!CreateDirectoryW(buffer, nullptr))
                throw FileError("Failed to create directory " + path);
        }

        return path;
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        return getStorageDirectoryApple(user);
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
                throw FileError("Failed to get home directory");
            else
                path = pwent.pw_dir;
        }

        path += DIRECTORY_SEPARATOR + "." + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
                throw FileError("Failed to create directory " + path);
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
                throw FileError("Failed to create directory " + path);
        }

        return path;
#elif OUZEL_PLATFORM_ANDROID
        OUZEL_UNUSED(user);

        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
        return engineAndroid->getFilesDirectory();
#else
        return "";
#endif
    }

    std::string FileSystem::getTempDirectory() const
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        char tempDirectory[1024];
        if (GetTempPathW(MAX_PATH, szBuffer))
        {
            if (WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, tempDirectory, sizeof(tempDirectory), nullptr, nullptr) == 0)
                throw FileError("Failed to convert wide char to UTF-8");

            return tempDirectory;
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
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        return engineAndroid->getCacheDirectory();
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
        if (!isAbsolutePath(filename))
        {
            EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

            AAsset* asset = AAssetManager_open(engineAndroid->getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

            if (!asset)
                throw FileError("Failed to open file " + filename);

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
            throw FileError("Failed to find file " + filename);

        File file(path, File::Mode::READ);

        while (uint32_t size = file.read(buffer, sizeof(buffer)))
            data.insert(data.end(), buffer, buffer + size);

        return data;
    }

    void FileSystem::writeFile(const std::string& filename, const std::vector<uint8_t>& data) const
    {
        File file(filename, File::Mode::WRITE | File::Mode::CREATE);

        uint32_t offset = 0;

        while (offset < data.size())
        {
            uint32_t written = file.write(data.data() + offset, static_cast<uint32_t>(data.size()) - offset);
            offset += written;
        }
    }

    bool FileSystem::resourceFileExists(const std::string& filename) const
    {
        if (isAbsolutePath(filename))
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
                    if (isAbsolutePath(path)) // if resource path is absolute
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
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        AAssetDir* assetDir = AAssetManager_openDir(engineAndroid->getAssetManager(), dirname.c_str());
        bool exists = AAssetDir_getNextFileName(assetDir) != nullptr;
        AAssetDir_close(assetDir);

        if (exists) return true;
#endif

        struct stat buf;
        if (stat(dirname.c_str(), &buf) != 0)
            return false;

        return (buf.st_mode & S_IFMT) == S_IFDIR;
    }

    bool FileSystem::fileExists(const std::string& filename) const
    {
#if OUZEL_PLATFORM_ANDROID
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        AAsset* asset = AAssetManager_open(engineAndroid->getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

        if (asset)
        {
            AAsset_close(asset);
            return true;
        }
#endif

        struct stat buf;
        if (stat(filename.c_str(), &buf) != 0)
            return false;

        return (buf.st_mode & S_IFMT) == S_IFREG;
    }

    std::string FileSystem::getPath(const std::string& filename, bool searchResources) const
    {
        if (isAbsolutePath(filename))
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
                    if (isAbsolutePath(path)) // if resource path is absolute
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

    bool FileSystem::isAbsolutePath(const std::string& path)
    {
#if OUZEL_PLATFORM_WINDOWS
        int size = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
        if (size == 0)
            throw FileError("Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> buffer(size);

        if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer.data(), size) == 0)
            throw FileError("Failed to convert UTF-8 to wide char");

        // relative paths longer than MAX_PATH are not supported
        if (buffer.size() > MAX_PATH)
            buffer.insert(buffer.begin(), { L'\\', L'\\', L'?', L'\\' });

        return PathIsRelativeW(buffer.data()) == FALSE;
#else
        return !path.empty() && path[0] == '/';
#endif
    }
}
