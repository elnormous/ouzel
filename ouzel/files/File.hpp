// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

#if OUZEL_PLATFORM_WINDOWS
#include <windows.h>
#else
#endif

namespace ouzel
{
    class File
    {
    public:
        File();
        File(const std::string& filename, int mode);
        ~File();

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        File(File&&);
        File& operator=(File&&);

        bool open(const std::string& filename, int mode);
        bool close();

        bool read(char* buffer, uint32_t& size);
        bool write(const char* buffer, uint32_t& size);
        bool seek(int32_t offset, int mode);

    protected:
#if OUZEL_PLATFORM_WINDOWS
        HANDLE file = INVALID_HANDLE_VALUE;
#else
        int fd = -1;
#if OUZEL_PLATFORM_ANDROID
        AAsset* asset = nullptr;
#endif
#endif
    };
}
