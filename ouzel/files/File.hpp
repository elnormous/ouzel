// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>

#include "core/Setup.h"

#if OUZEL_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#if OUZEL_PLATFORM_ANDROID
#include <android/asset_manager_jni.h>
#endif

namespace ouzel
{
    class File final
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

        inline bool isOpen() const
        {
#if OUZEL_PLATFORM_WINDOWS
            return file != INVALID_HANDLE_VALUE;
#else
            return fd != -1;
#endif
        }

        uint32_t read(void* buffer, uint32_t size, bool all = false) const;
        uint32_t write(const void* buffer, uint32_t size, bool all = false) const;
        void seek(int32_t offset, int method) const;
        uint32_t getOffset() const;

    private:
#if OUZEL_PLATFORM_WINDOWS
        HANDLE file = INVALID_HANDLE_VALUE;
#else
        int fd = -1;
#endif
    };
}
