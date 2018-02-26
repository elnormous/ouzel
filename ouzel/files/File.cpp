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
        if (file == INVALID_HANDLE_VALUE) return false;
        return CloseHandle(file) != 0;
#else
        if (fd == -1) return false;
        return ::close(fd) == 0;
#endif
    }

    bool File::read(void* buffer, uint32_t size, uint32_t& bytesRead)
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file == INVALID_HANDLE_VALUE) return false;
        return ReadFile(file, buffer, size, &bytesRead, nullptr) != 0;
#else
        if (fd == -1) return false;
        ssize_t ret = ::read(fd, buffer, size);
        bytesRead = static_cast<uint32_t>(ret);
        return ret != -1;
#endif
    }

    bool File::write(const void* buffer, uint32_t size, uint32_t& bytesWritten)
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file == INVALID_HANDLE_VALUE) return false;
        return WriteFile(file, buffer, size, &bytesWritten, nullptr) != 0;
#else
        if (fd == -1) return false;
        ssize_t ret = ::write(fd, buffer, size);
        bytesWritten = static_cast<uint32_t>(ret);
        return ret != -1;
#endif
    }

    bool File::seek(int32_t offset, int method)
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file == INVALID_HANDLE_VALUE) false;
        DWORD moveMethod = 0;
        if (method == BEGIN) moveMethod = FILE_BEGIN;
        else if (method == CURRENT) moveMethod = FILE_CURRENT;
        else if (method == END) moveMethod = FILE_END;
        return SetFilePointer(file, offset, nullptr, moveMethod) != 0;
#else
        if (fd == -1) return false;
        int whence = 0;
        if (method == BEGIN) whence = SEEK_SET;
        else if (method == CURRENT) whence = SEEK_CUR;
        else if (method == END) whence = SEEK_END;
        return lseek(fd, offset, whence) != -1;
#endif
    }
}
