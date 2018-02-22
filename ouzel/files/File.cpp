// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_WINDOWS
#else
#include <fcntl.h>
#include <unistd.h>
#endif
#include "File.hpp"

namespace ouzel
{
    File::File()
    {
    }

    File::File(const std::string& filename, int mode)
    {
        open(filename, mode);
    }

    File::~File()
    {
        close();
    }

    File::File(File&& other)
    {
#if OUZEL_PLATFORM_WINDOWS
        file = other.file;
        other.file = nullptr;
#else
        fd = other.fd;
        other.fd = -1;
#endif
    }

    File& File::operator=(File&& other)
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        file = other.file;
        other.file = nullptr;
#else
        if (fd != -1) ::close(fd);
        fd = other.fd;
        other.fd = -1;
#endif

        return *this;
    }

    bool File::open(const std::string& filename, int mode)
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, filename.c_str(), filename.length(), nullptr, nullptr);
        file = CreateFile(szBuffer, GENERIC_READ, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        return file != INVALID_HANDLE_VALUE;
#else
        fd = ::open(filename.c_str(), O_RDONLY);
        return fd != -1;
#endif
    }

    bool File::close()
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file != INVALID_HANDLE_VALUE) return CloseHandle(file) != 0;
#else
        if (fd != -1) return ::close(fd) == 0;
#endif
        return true;
    }

    bool File::read(char* buffer, uint32_t& size)
    {
#if OUZEL_PLATFORM_WINDOWS
        //if (file != INVALID_HANDLE_VALUE) return ReadFile(file) != 0;
#else
        //if (fd != -1) return ::read(fd, buffer, size) == 0;
#endif
        return true;
    }

    bool File::write(const char* buffer, uint32_t& size)
    {
#if OUZEL_PLATFORM_WINDOWS
        //if (file != INVALID_HANDLE_VALUE) return WriteFile(file) != 0;
#else
        //if (fd != -1) return ::write(fd, buffer, size) == 0;
#endif
        return true;
    }

    bool File::seek(int32_t offset, int mode)
    {
#if OUZEL_PLATFORM_WINDOWS
        //if (file) return SetFilePointer(file, FILE_BEGIN) != 0;
#else
        //if (fd != -1) return lseek(fd, offset, SEEK_SET) == 0;
#endif
        return true;
    }
}
