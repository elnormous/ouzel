// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if !OUZEL_PLATFORM_WINDOWS
#include <fcntl.h>
#include <unistd.h>
#endif
#include "File.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    File::File()
    {
    }

    File::File(const std::string& filename, int mode)
    {
#if OUZEL_PLATFORM_WINDOWS
        DWORD access = 0;
        if (mode & READ) access |= GENERIC_READ;
        if (mode & WRITE) access |= GENERIC_WRITE;
        if (mode & APPEND) access |= FILE_APPEND_DATA;
        DWORD createDisposition = (mode & CREATE) ? OPEN_ALWAYS : OPEN_EXISTING;

        WCHAR buffer[MAX_PATH];
        if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer, MAX_PATH) == 0)
            throw FileError("Failed to convert the filename to wide char");

        file = CreateFileW(buffer, access, 0, nullptr, createDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
            throw FileError("Failed to open file");
#else
        int access = 0;
        if ((mode & READ) && (mode & WRITE)) access |= O_RDWR;
        else if (mode & READ) access |= O_RDONLY;
        else if (mode & WRITE) access |= O_WRONLY;
        if (mode & CREATE) access |= O_CREAT;
        if (mode & APPEND) access |= O_APPEND;

        fd = open(filename.c_str(), access, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1)
            throw FileError("Failed to open file");
#endif
    }

    File::~File()
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
#else
        if (fd != -1) close(fd);
#endif
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
        if (&other != this)
        {
#if OUZEL_PLATFORM_WINDOWS
            if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
            file = other.file;
            other.file = nullptr;
#else
            if (fd != -1) close(fd);
            fd = other.fd;
            other.fd = -1;
#endif
        }

        return *this;
    }

    uint32_t File::read(void* buffer, uint32_t size, bool all) const
    {
        if (all)
        {
            uint8_t* dest = static_cast<uint8_t*>(buffer);
            uint32_t remaining = size;

            while (remaining > 0)
            {
                uint32_t bytesRead = read(dest, remaining);

                if (bytesRead == 0)
                    throw FileError("End of file reached");

                remaining -= bytesRead;
                dest += bytesRead;
            }

            return size;
        }
        else
        {
#if OUZEL_PLATFORM_WINDOWS
            if (file == INVALID_HANDLE_VALUE)
                throw FileError("File is not open");

            DWORD n;
            if (!ReadFile(file, buffer, size, &n, nullptr))
                throw FileError("Failed to read from file");

            return static_cast<uint32_t>(n);
#else
            if (fd == -1)
                throw FileError("File is not open");

            ssize_t ret = ::read(fd, buffer, size);

            if (ret == -1)
                throw FileError("Failed to read from file");

            return static_cast<uint32_t>(ret);
#endif
        }
    }

    uint32_t File::write(const void* buffer, uint32_t size, bool all) const
    {
        if (all)
        {
            const uint8_t* src = static_cast<const uint8_t*>(buffer);
            uint32_t remaining = size;

            while (remaining > 0)
            {
                uint32_t bytesWritten = write(src, remaining);
                remaining -= bytesWritten;
                src += bytesWritten;
            }

            return size;
        }
        else
        {
#if OUZEL_PLATFORM_WINDOWS
            if (file == INVALID_HANDLE_VALUE)
                throw FileError("File is not open");

            DWORD n;
            if (!WriteFile(file, buffer, size, &n, nullptr))
                throw FileError("Failed to write to file");

            return static_cast<uint32_t>(n);
#else
            if (fd == -1)
                throw FileError("File is not open");

            ssize_t ret = ::write(fd, buffer, size);

            if (ret == -1)
                throw FileError("Failed to write to file");

            return static_cast<uint32_t>(ret);
#endif
        }
    }

    void File::seek(int32_t offset, int method) const
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file == INVALID_HANDLE_VALUE)
            throw FileError("File is not open");

        DWORD moveMethod = 0;
        if (method == BEGIN) moveMethod = FILE_BEGIN;
        else if (method == CURRENT) moveMethod = FILE_CURRENT;
        else if (method == END) moveMethod = FILE_END;
        if (SetFilePointer(file, offset, nullptr, moveMethod) == 0)
            throw FileError("Failed to seek file");
#else
        if (fd == -1)
            throw FileError("File is not open");

        int whence = 0;
        if (method == BEGIN) whence = SEEK_SET;
        else if (method == CURRENT) whence = SEEK_CUR;
        else if (method == END) whence = SEEK_END;
        if (lseek(fd, offset, whence) == -1)
            throw FileError("Failed to seek file");
#endif
    }

    uint32_t File::getOffset() const
    {
#if OUZEL_PLATFORM_WINDOWS
        if (file == INVALID_HANDLE_VALUE) return 0;
        DWORD ret = SetFilePointer(file, 0, nullptr, FILE_CURRENT);
        return static_cast<uint32_t>(ret);
#else
        if (fd == -1) return 0;
        off_t ret = lseek(fd, 0, SEEK_CUR);
        if (ret == -1) return 0;
        return static_cast<uint32_t>(ret);
#endif
    }
}
