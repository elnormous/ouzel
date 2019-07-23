// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_FILE_HPP
#define OUZEL_STORAGE_FILE_HPP

#include <string>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#else
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#if defined(__ANDROID__)
#  include <android/asset_manager_jni.h>
#endif

namespace ouzel
{
    namespace storage
    {
        class File final
        {
        public:
            enum Mode
            {
                Read = 0x01,
                Write = 0x02,
                Create = 0x04,
                Append = 0x08,
                Truncate = 0x10
            };

            enum class Seek
            {
                Begin,
                Current,
                End
            };

            File() noexcept = default;
            File(const std::string& filename, int mode)
            {
#if defined(_WIN32)
                DWORD access = 0;
                if (mode & Mode::Read) access |= GENERIC_READ;
                if (mode & Mode::Write) access |= GENERIC_WRITE;
                if (mode & Mode::Append) access |= FILE_APPEND_DATA;
                DWORD createDisposition = 0;
                if (mode & Truncate)
                    createDisposition = (mode & Mode::Create) ? CREATE_ALWAYS : TRUNCATE_EXISTING;
                else
                    createDisposition = (mode & Mode::Create) ? OPEN_ALWAYS : OPEN_EXISTING;

                const int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0);
                if (bufferSize == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

                std::vector<WCHAR> buffer(bufferSize);
                if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer.data(), bufferSize) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert the filename to wide char");

                // relative paths longer than MAX_PATH are not supported
                if (buffer.size() > MAX_PATH)
                    buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

                file = CreateFileW(buffer.data(), access, 0, nullptr, createDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");
#else
                int access = 0;
                if ((mode & Mode::Read) && (mode & Mode::Write)) access |= O_RDWR;
                else if (mode & Mode::Read) access |= O_RDONLY;
                else if (mode & Mode::Write) access |= O_WRONLY;
                if (mode & Mode::Create) access |= O_CREAT;
                if (mode & Mode::Append) access |= O_APPEND;
                if (mode & Mode::Truncate) access |= O_TRUNC;

                file = open(filename.c_str(), access, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (file == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to open file");
#endif
            }

            ~File()
            {
#if defined(_WIN32)
                if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
#else
                if (file != -1) ::close(file);
#endif
            }

            File(const File&) = delete;
            File& operator=(const File&) = delete;

            File(File&& other) noexcept
            {
#if defined(_WIN32)
                file = other.file;
                other.file = nullptr;
#else
                file = other.file;
                other.file = -1;
#endif
            }

            File& operator=(File&& other) noexcept
            {
                if (&other == this) return *this;

#if defined(_WIN32)
                if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
                file = other.file;
                other.file = nullptr;
#else
                if (file != -1) ::close(file);
                file = other.file;
                other.file = -1;
#endif

                return *this;
            }

            inline auto isOpen() const noexcept
            {
#if defined(_WIN32)
                return file != INVALID_HANDLE_VALUE;
#else
                return file != -1;
#endif
            }

            void close()
            {
#if defined(_WIN32)
                HANDLE f = file;
                file = INVALID_HANDLE_VALUE;
                if (!CloseHandle(f))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to close file");
#else
                int f = file;
                file = -1;
                if (::close(f) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to close file");
#endif
            }

            uint32_t read(void* buffer, uint32_t size, bool all = false) const
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
                    ssize_t ret = ::read(file, buffer, size);

                    if (ret == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to read from file");

                    return static_cast<uint32_t>(ret);
#endif
                }
            }

            uint32_t write(const void* buffer, uint32_t size, bool all = false) const
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
                    ssize_t ret = ::write(file, buffer, size);

                    if (ret == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to write to file");

                    return static_cast<uint32_t>(ret);
#endif
                }
            }

            void seek(int32_t offset, Seek method) const
            {
#if defined(_WIN32)
                DWORD moveMethod = 0;
                switch (method)
                {
                    case Seek::Begin: moveMethod = FILE_BEGIN; break;
                    case Seek::Current: moveMethod = FILE_CURRENT; break;
                    case Seek::End: moveMethod = FILE_END; break;
                    default:
                        throw std::runtime_error("Unsupported seek method");

                }
                if (SetFilePointer(file, offset, nullptr, moveMethod) == INVALID_SET_FILE_POINTER)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to seek file");
#else
                int whence = 0;
                switch (method)
                {
                    case Seek::Begin: whence = SEEK_SET; break;
                    case Seek::Current: whence = SEEK_CUR; break;
                    case Seek::End: whence = SEEK_END; break;
                    default:
                        throw std::runtime_error("Unsupported seek method");

                }
                if (lseek(file, offset, whence) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to seek file");
#endif
            }

            auto getOffset() const
            {
#if defined(_WIN32)
                DWORD ret = SetFilePointer(file, 0, nullptr, FILE_CURRENT);
                if (ret == INVALID_SET_FILE_POINTER)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to seek file");
                return static_cast<uint32_t>(ret);
#else
                off_t ret = lseek(file, 0, SEEK_CUR);
                if (ret == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to seek file");
                return static_cast<uint32_t>(ret);
#endif
            }

        private:
#if defined(_WIN32)
            HANDLE file = INVALID_HANDLE_VALUE;
#else
            int file = -1;
#endif
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_FILE_HPP
