// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>

#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(__ANDROID__)
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
            APPEND = 0x08,
            TRUNCATE = 0x10
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
#if defined(_WIN32)
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
#if defined(_WIN32)
        HANDLE file = INVALID_HANDLE_VALUE;
#else
        int fd = -1;
#endif
    };
}
