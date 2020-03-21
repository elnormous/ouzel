// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <system_error>
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
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__APPLE__)
#  include <TargetConditionals.h>
#  include <objc/message.h>
#  include <objc/NSObjCRuntime.h>
#  include <CoreFoundation/CoreFoundation.h>
#elif defined(__ANDROID__)
#  include "core/android/EngineAndroid.hpp"
#elif defined(__linux__)
#  include <limits.h>
#  include <pwd.h>
#  include <unistd.h>
#endif

#if defined(__unix__) || defined(__APPLE__)
#  include <sys/stat.h>
#endif

#include "FileSystem.hpp"
#include "Archive.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"

#if defined(__APPLE__)
#  include "CfPointer.hpp"
extern "C" id NSTemporaryDirectory();
#endif

namespace ouzel
{
    namespace storage
    {
        FileSystem::FileSystem(Engine& initEngine):
            engine(initEngine)
        {
#if defined(_WIN32)
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

            const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, nullptr, 0, nullptr, nullptr);
            if (bufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

            std::vector<char> appFilename(bufferSize);
            if (WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, appFilename.data(), bufferSize, nullptr, nullptr) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

            appPath = Path(appFilename.data()).getDirectory();
            engine.log(Log::Level::Info) << "Application directory: " << appPath;

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

            appPath = resourceDirectory.data();
            engine.log(Log::Level::Info) << "Application directory: " << appPath;

#elif defined(__ANDROID__)
            // not available for Android

#elif defined(__linux__)
            char executableDirectory[PATH_MAX];

            ssize_t length;
            if ((length = readlink("/proc/self/exe", executableDirectory, sizeof(executableDirectory) - 1)) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to get current directory");

            executableDirectory[length] = '\0';
            appPath = Path(executableDirectory).getDirectory();
            engine.log(Log::Level::Info) << "Application directory: " << appPath;
#endif
        }

        std::string FileSystem::getStorageDirectory(const bool user) const
        {
#if defined(_WIN32)
            WCHAR appDataPath[MAX_PATH];

            HRESULT hr;
            if (FAILED(hr = SHGetFolderPathW(nullptr, (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, appDataPath)))
                throw std::system_error(hr, std::system_category(), "Failed to get the path of the AppData directory");

            const int appDataBufferSize = WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, nullptr, 0, nullptr, nullptr);
            if (appDataBufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

            std::vector<char> appDataBuffer(appDataBufferSize);
            if (WideCharToMultiByte(CP_UTF8, 0, appDataPath, -1, appDataBuffer.data(), appDataBufferSize, nullptr, nullptr) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

            std::string path = appDataBuffer.data();

            path += Path::directorySeparator + OUZEL_DEVELOPER_NAME;

            if (!directoryExists(path))
            {
                const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
                if (bufferSize == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

                std::vector<WCHAR> buffer(bufferSize);
                if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer.data(), bufferSize) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

                // relative paths longer than MAX_PATH are not supported
                if (buffer.size() > MAX_PATH)
                    buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

                const DWORD attributes = GetFileAttributesW(buffer.data());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                {
                    if (!CreateDirectoryW(buffer.data(), nullptr))
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to create directory " + path);
                }
                else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    throw std::runtime_error(path + " is not a directory");
            }

            path += Path::directorySeparator + OUZEL_APPLICATION_NAME;

            if (!directoryExists(path))
            {
                const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
                if (bufferSize == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

                std::vector<WCHAR> buffer(bufferSize);
                if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, buffer.data(), bufferSize) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

                // relative paths longer than MAX_PATH are not supported
                if (buffer.size() > MAX_PATH)
                    buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

                const DWORD attributes = GetFileAttributesW(buffer.data());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                {
                    if (!CreateDirectoryW(buffer.data(), nullptr))
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to create directory " + path);
                }
                else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    throw std::runtime_error(path + " is not a directory");
            }

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
            return reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(documentDirectoryString, sel_getUid("UTF8String"));
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
            return reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(pathString, sel_getUid("UTF8String"));
#elif defined(__ANDROID__)
            static_cast<void>(user);

            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);
            return engineAndroid.getFilesDirectory();
#elif defined(__linux__)
            std::string path;

            const char* homeDirectory = getenv("XDG_DATA_HOME");

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

                path += Path::directorySeparator + ".local";

                if (!directoryExists(path))
                    if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);

                path += Path::directorySeparator + "share";

                if (!directoryExists(path))
                    if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);
            }

            path += Path::directorySeparator + OUZEL_DEVELOPER_NAME;

            if (!directoryExists(path))
                if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);

            path += Path::directorySeparator + OUZEL_APPLICATION_NAME;

            if (!directoryExists(path))
                if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to create directory " + path);

            return path;
#else
            return "";
#endif
        }

        std::string FileSystem::getTempDirectory() const
        {
#if defined(_WIN32)
            WCHAR buffer[MAX_PATH];
            if (GetTempPathW(MAX_PATH, buffer))
            {
                const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
                if (bufferSize == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                std::vector<char> tempDirectoryBuffer(bufferSize);
                if (WideCharToMultiByte(CP_UTF8, 0, buffer, -1, tempDirectoryBuffer.data(), bufferSize, nullptr, nullptr) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                return tempDirectoryBuffer.data();
            }
            else
                return "";
#elif defined(__APPLE__)
            id temporaryDirectory = NSTemporaryDirectory();
            return reinterpret_cast<const char* (*)(id, SEL)>(&objc_msgSend)(temporaryDirectory, sel_getUid("UTF8String")); // [temporaryDirectory UTF8String]
#elif defined(__ANDROID__)
            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);
            return engineAndroid.getCacheDirectory();
#elif defined(__linux__)
            char const* path = getenv("TMPDIR");
            if (path)
                return path;
            else
                return "/tmp";
#else
            return "";
#endif
        }

        std::vector<std::uint8_t> FileSystem::readFile(const std::string& filename, const bool searchResources)
        {
            if (searchResources)
                for (auto& archive : archives)
                    if (archive.second.fileExists(filename))
                        return archive.second.readFile(filename);

#if defined(__ANDROID__)
            if (!Path(filename).isAbsolute())
            {
                EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

                AAsset* asset = AAssetManager_open(engineAndroid.getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

                if (!asset)
                    throw std::runtime_error("Failed to open file " + filename);

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
            if (path.empty())
                throw std::runtime_error("Failed to find file " + filename);

            std::ifstream f(path, std::ios::binary);
            return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        }

        void FileSystem::writeFile(const std::string& filename, const std::vector<std::uint8_t>& data) const
        {
            std::ofstream f(filename, std::ios::binary | std::ios::trunc);
            f.write(reinterpret_cast<const char*>(data.data()),
                    static_cast<std::streamsize>(data.size()));
        }

        bool FileSystem::resourceFileExists(const std::string& filename) const
        {
            if (Path(filename).isAbsolute())
                return fileExists(filename);
            else
            {
                std::string str = appPath + Path::directorySeparator + filename;

                if (fileExists(str))
                    return true;
                else
                    for (const std::string& path : resourcePaths)
                    {
                        if (Path(path).isAbsolute()) // if resource path is absolute
                            str = path + Path::directorySeparator + filename;
                        else
                            str = appPath + Path::directorySeparator + path + Path::directorySeparator + filename;

                        if (fileExists(str))
                            return true;
                    }

                return false;
            }
        }

        bool FileSystem::directoryExists(const std::string& dirname) const
        {
#if defined(__ANDROID__)
            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

            AAssetDir* assetDir = AAssetManager_openDir(engineAndroid.getAssetManager(), dirname.c_str());
            const bool exists = AAssetDir_getNextFileName(assetDir) != nullptr;
            AAssetDir_close(assetDir);

            if (exists) return true;
#endif

#if defined(_WIN32)
            const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, dirname.c_str(), -1, nullptr, 0);
            if (bufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> buffer(bufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, dirname.c_str(), -1, buffer.data(), bufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            // relative paths longer than MAX_PATH are not supported
            if (buffer.size() > MAX_PATH)
                buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

            const DWORD attributes = GetFileAttributesW(buffer.data());
            if (attributes == INVALID_FILE_ATTRIBUTES)
                return false;

            return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif defined(__unix__) || defined(__APPLE__)
            struct stat buf;
            if (stat(dirname.c_str(), &buf) == -1)
                return false;

            return (buf.st_mode & S_IFMT) == S_IFDIR;
#endif
        }

        bool FileSystem::fileExists(const std::string& filename) const
        {
#if defined(__ANDROID__)
            EngineAndroid& engineAndroid = static_cast<EngineAndroid&>(engine);

            AAsset* asset = AAssetManager_open(engineAndroid.getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

            if (asset)
            {
                AAsset_close(asset);
                return true;
            }
#endif

#if defined(_WIN32)
            const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0);
            if (bufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> buffer(bufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer.data(), bufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            // relative paths longer than MAX_PATH are not supported
            if (buffer.size() > MAX_PATH)
                buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

            const DWORD attributes = GetFileAttributesW(buffer.data());
            if (attributes == INVALID_FILE_ATTRIBUTES)
                return false;

            return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#elif defined(__unix__) || defined(__APPLE__)
            struct stat buf;
            if (stat(filename.c_str(), &buf) == -1)
                return false;

            return (buf.st_mode & S_IFMT) == S_IFREG;
#endif
        }
    } // namespace storage
} // namespace ouzel
