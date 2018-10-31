// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <system_error>
#if !defined(_WIN32)
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <vector>
#include "File.hpp"

namespace ouzel
{
    File::File()
    {
    }

    File::File(const std::string& filename, int mode)
    {
#if defined(_WIN32)
        DWORD access = 0;
        if (mode & READ) access |= GENERIC_READ;
        if (mode & WRITE) access |= GENERIC_WRITE;
        if (mode & APPEND) access |= FILE_APPEND_DATA;
        DWORD createDisposition = 0;
        if (mode & TRUNCATE)
            createDisposition = (mode & CREATE) ? CREATE_ALWAYS : TRUNCATE_EXISTING;
        else
            createDisposition = (mode & CREATE) ? OPEN_ALWAYS : OPEN_EXISTING;

        int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0);
        if (bufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> buffer(bufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer.data(), bufferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert the filename to wide char");

        // relative paths longer than MAX_PATH are not supported
        if (buffer.size() > MAX_PATH)
            buffer.insert(buffer.begin(), { L'\\', L'\\', L'?', L'\\' });

        file = CreateFileW(buffer.data(), access, 0, nullptr, createDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");
#else
        int access = 0;
        if ((mode & READ) && (mode & WRITE)) access |= O_RDWR;
        else if (mode & READ) access |= O_RDONLY;
        else if (mode & WRITE) access |= O_WRONLY;
        if (mode & CREATE) access |= O_CREAT;
        if (mode & APPEND) access |= O_APPEND;
        if (mode & TRUNCATE) access |= O_TRUNC;

        fd = open(filename.c_str(), access, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1)
            throw std::system_error(errno, std::system_category(), "Failed to open file");
#endif
    }

    File::~File()
    {
#if defined(_WIN32)
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
#else
        if (fd != -1) close(fd);
#endif
    }

    File::File(File&& other)
    {
#if defined(_WIN32)
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
#if defined(_WIN32)
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
                    return 0; // End of file reached

                remaining -= bytesRead;
                dest += bytesRead;
            }

            return size;
        }
        else
        {
#if defined(_WIN32)
            DWORD n;
            if (!ReadFile(file, buffer, size, &n, nullptr))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to read from file");

            return static_cast<uint32_t>(n);
#else
            ssize_t ret = ::read(fd, buffer, size);

            if (ret == -1)
                throw std::system_error(errno, std::system_category(), "Failed to read from file");

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
                uint32_t bytesWritten = write(src, remaining, false);
                remaining -= bytesWritten;
                src += bytesWritten;
            }

            return size;
        }
        else
        {
#if defined(_WIN32)
            DWORD n;
            if (!WriteFile(file, buffer, size, &n, nullptr))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to write to file");

            return static_cast<uint32_t>(n);
#else
            ssize_t ret = ::write(fd, buffer, size);

            if (ret == -1)
                throw std::system_error(errno, std::system_category(), "Failed to write to file");

            return static_cast<uint32_t>(ret);
#endif
        }
    }

    void File::seek(int32_t offset, int method) const
    {
#if defined(_WIN32)
        DWORD moveMethod = 0;
        if (method == BEGIN) moveMethod = FILE_BEGIN;
        else if (method == CURRENT) moveMethod = FILE_CURRENT;
        else if (method == END) moveMethod = FILE_END;
        if (SetFilePointer(file, offset, nullptr, moveMethod) == INVALID_SET_FILE_POINTER)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to seek file");
#else
        int whence = 0;
        if (method == BEGIN) whence = SEEK_SET;
        else if (method == CURRENT) whence = SEEK_CUR;
        else if (method == END) whence = SEEK_END;
        if (lseek(fd, offset, whence) == -1)
            throw std::system_error(errno, std::system_category(), "Failed to seek file");
#endif
    }

    uint32_t File::getOffset() const
    {
#if defined(_WIN32)
        DWORD ret = SetFilePointer(file, 0, nullptr, FILE_CURRENT);
        if (ret == INVALID_SET_FILE_POINTER)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to seek file");
        return static_cast<uint32_t>(ret);
#else
        off_t ret = lseek(fd, 0, SEEK_CUR);
        if (ret == -1)
            throw std::system_error(errno, std::system_category(), "Failed to seek file");
        return static_cast<uint32_t>(ret);
#endif
    }
}
