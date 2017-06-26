// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include "FileSystemWin.h"
#include "utils/Log.h"

extern std::string DEVELOPER_NAME;
extern std::string APPLICATION_NAME;
static char TEMP_BUFFER[1024];

namespace ouzel
{
    FileSystemWin::FileSystemWin()
    {
        char* exePath = _pgmptr;

        if (exePath)
        {
            appPath = getDirectoryPart(exePath);
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
    }

    std::string FileSystemWin::getHomeDirectory() const
    {
        WCHAR szBuffer[MAX_PATH];
        HRESULT hr = SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, SHGFP_TYPE_CURRENT, szBuffer);
        if (FAILED(hr))
        {
            Log(Log::Level::ERR) << "Failed to get the path of the profile directory, error: " << hr;
            return "";
        }

        WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
        return TEMP_BUFFER;
    }

    std::string FileSystemWin::getStorageDirectory(bool user) const
    {
        WCHAR szBuffer[MAX_PATH];
        HRESULT hr = SHGetFolderPathW(nullptr, (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, szBuffer);
        if (FAILED(hr))
        {
            Log(Log::Level::ERR) << "Failed to get the path of the AppData directory, error: " << hr;
            return "";
        }

        WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
        path = TEMP_BUFFER;

        path += DIRECTORY_SEPARATOR + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            if (!CreateDirectoryW(szBuffer, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            if (!CreateDirectoryW(szBuffer, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        return path;
    }

    std::string FileSystemWin::getTempDirectory() const
    {
        WCHAR szBuffer[MAX_PATH];
        if (GetTempPathW(MAX_PATH, szBuffer))
        {
            WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);

            return TEMP_BUFFER;
        }

        return "";
    }

    bool FileSystemWin::isAbsolutePath(const std::string& path) const
    {
        WCHAR szBuffer[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
        return PathIsRelativeW(szBuffer) == FALSE;
    }
}
