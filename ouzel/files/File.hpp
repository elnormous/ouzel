// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

#if OUZEL_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if OUZEL_PLATFORM_ANDROID
#include <android/asset_manager_jni.h>
#endif

namespace ouzel
{
    class File
    {
    public:
        enum Mode
        {
            READ = 0x01,
            WRITE = 0x02,
            CREATE = 0x04,
            APPEND = 0x08
        };

        enum Seek
        {
            BEGIN,
            CURRENT,
            END
        };

        File();
        File(const std::string& filename, int mode);
        ~File();

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        File(File&&);
        File& operator=(File&&);

        bool open(const std::string& filename, int mode);
        bool close();

        inline bool isOpen() const
        {
#if OUZEL_PLATFORM_WINDOWS
            return file != INVALID_HANDLE_VALUE;
#else
            return fd != -1;
#endif
        }

        bool read(void* buffer, uint32_t& size);
        bool write(const void* buffer, uint32_t& size);
        bool seek(int32_t offset, int method);

    protected:
#if OUZEL_PLATFORM_WINDOWS
        HANDLE file = INVALID_HANDLE_VALUE;
#else
        int fd = -1;
#endif

#if OUZEL_PLATFORM_ANDROID
        AAsset* asset = nullptr;
#endif
    };
}
