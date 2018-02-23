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
        DWORD access = 0;
        if (mode & READ) access |= GENERIC_READ;
        if (mode & WRITE) access |= GENERIC_WRITE;
        if (mode & APPEND) access |= FILE_APPEND_DATA;
        DWORD createDisposition = (mode & CREATE) ? OPEN_ALWAYS : OPEN_EXISTING;

        WCHAR buffer[MAX_PATH];
        if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer, MAX_PATH) == 0)
            return false;
        file = CreateFile(buffer, access, 0, nullptr, createDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
        return file != INVALID_HANDLE_VALUE;
#else
        int access = 0;
        if ((mode & READ) && (mode & WRITE)) access |= O_RDWR;
        else if (mode & READ) access |= O_RDONLY;
        else if (mode & WRITE) access |= O_WRONLY;
        if (mode & CREATE) access |= O_CREAT;
        if (mode & APPEND) access |= O_APPEND;

        fd = ::open(filename.c_str(), access);
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
