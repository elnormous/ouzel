#include "AtomicFile.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>

#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#else
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

namespace fluxion::storage
{
    namespace
    {
#if defined(_WIN32)
        // Build "<path>.tmp" in the platform string type so we don't pay
        // a UTF-8 round trip per save and so unicode paths survive intact.
        Path::String tempPathFor(const Path& path)
        {
            Path::String s = path.getNative();
            s.append(L".tmp");
            return s;
        }
#else
        Path::String tempPathFor(const Path& path)
        {
            Path::String s = path.getNative();
            s.append(".tmp");
            return s;
        }
#endif

        [[noreturn]] void throwSystemError(const char* what)
        {
#if defined(_WIN32)
            throw std::system_error{static_cast<int>(GetLastError()),
                                    std::system_category(), what};
#else
            throw std::system_error{errno, std::generic_category(), what};
#endif
        }
    }

    void atomicWriteFile(const Path& path, const void* data, std::size_t size)
    {
        const auto tempPath = tempPathFor(path);

#if defined(_WIN32)
        const HANDLE handle = CreateFileW(tempPath.c_str(),
                                          GENERIC_WRITE,
                                          0,                        // exclusive access
                                          nullptr,                  // default security
                                          CREATE_ALWAYS,            // overwrite if leftover from a previous crash
                                          FILE_ATTRIBUTE_NORMAL,
                                          nullptr);
        if (handle == INVALID_HANDLE_VALUE) throwSystemError("AtomicWrite: CreateFileW failed");

        // Write the buffer; loop because WriteFile can write fewer bytes than asked
        // (rare for local files but allowed by the API contract).
        const auto bytes = static_cast<const char*>(data);
        std::size_t written = 0;
        while (written < size)
        {
            const DWORD chunk = static_cast<DWORD>(
                std::min<std::size_t>(size - written, 0x40000000U));
            DWORD actual = 0;
            if (!WriteFile(handle, bytes + written, chunk, &actual, nullptr) || actual == 0)
            {
                const DWORD err = GetLastError();
                CloseHandle(handle);
                DeleteFileW(tempPath.c_str());
                throw std::system_error{static_cast<int>(err), std::system_category(),
                                        "AtomicWrite: WriteFile failed"};
            }
            written += actual;
        }

        // Force the write to physical media before the rename. Without
        // this, a power loss after rename could yield the new path entry
        // pointing at a buffer-pool block that never reached disk.
        if (!FlushFileBuffers(handle))
        {
            const DWORD err = GetLastError();
            CloseHandle(handle);
            DeleteFileW(tempPath.c_str());
            throw std::system_error{static_cast<int>(err), std::system_category(),
                                    "AtomicWrite: FlushFileBuffers failed"};
        }
        CloseHandle(handle);

        if (!MoveFileExW(tempPath.c_str(),
                         path.getNative().c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            const DWORD err = GetLastError();
            DeleteFileW(tempPath.c_str());
            throw std::system_error{static_cast<int>(err), std::system_category(),
                                    "AtomicWrite: MoveFileExW failed"};
        }
#else
        // O_CLOEXEC keeps the fd from leaking through fork/exec, which is
        // cheap insurance even though the editor isn't expected to fork.
        const int fd = ::open(tempPath.c_str(),
                              O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
                              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0) throwSystemError("AtomicWrite: open failed");

        const auto bytes = static_cast<const char*>(data);
        std::size_t written = 0;
        while (written < size)
        {
            const ssize_t n = ::write(fd, bytes + written, size - written);
            if (n < 0)
            {
                if (errno == EINTR) continue;
                const int err = errno;
                ::close(fd);
                ::unlink(tempPath.c_str());
                throw std::system_error{err, std::generic_category(),
                                        "AtomicWrite: write failed"};
            }
            if (n == 0) break;  // shouldn't happen for a regular file
            written += static_cast<std::size_t>(n);
        }

        // fsync(fd) on macOS is documented to *not* flush all the way to
        // physical media; that needs F_FULLFSYNC. We accept the macOS
        // weaker guarantee here in exchange for keeping the same call
        // shape across POSIX targets — F_FULLFSYNC is significantly
        // slower and most editors live with fsync semantics.
#  ifndef __EMSCRIPTEN__
        if (::fsync(fd) != 0)
        {
            const int err = errno;
            ::close(fd);
            ::unlink(tempPath.c_str());
            throw std::system_error{err, std::generic_category(),
                                    "AtomicWrite: fsync failed"};
        }
#  endif
        ::close(fd);

        if (::rename(tempPath.c_str(), path.getNative().c_str()) != 0)
        {
            const int err = errno;
            ::unlink(tempPath.c_str());
            throw std::system_error{err, std::generic_category(),
                                    "AtomicWrite: rename failed"};
        }
#endif
    }

    std::vector<std::byte> readBinaryFile(const Path& path)
    {
#if defined(_WIN32)
        const HANDLE handle = CreateFileW(path.getNative().c_str(),
                                          GENERIC_READ,
                                          FILE_SHARE_READ,
                                          nullptr,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL,
                                          nullptr);
        if (handle == INVALID_HANDLE_VALUE) throwSystemError("readBinaryFile: CreateFileW failed");

        LARGE_INTEGER size{};
        if (!GetFileSizeEx(handle, &size))
        {
            const DWORD err = GetLastError();
            CloseHandle(handle);
            throw std::system_error{static_cast<int>(err), std::system_category(),
                                    "readBinaryFile: GetFileSizeEx failed"};
        }

        std::vector<std::byte> result(static_cast<std::size_t>(size.QuadPart));
        std::size_t read = 0;
        while (read < result.size())
        {
            const DWORD chunk = static_cast<DWORD>(
                std::min<std::size_t>(result.size() - read, 0x40000000U));
            DWORD actual = 0;
            if (!ReadFile(handle, result.data() + read, chunk, &actual, nullptr))
            {
                const DWORD err = GetLastError();
                CloseHandle(handle);
                throw std::system_error{static_cast<int>(err), std::system_category(),
                                        "readBinaryFile: ReadFile failed"};
            }
            if (actual == 0) break;
            read += actual;
        }
        CloseHandle(handle);
        result.resize(read);
        return result;
#else
        const int fd = ::open(path.getNative().c_str(), O_RDONLY | O_CLOEXEC);
        if (fd < 0) throwSystemError("readBinaryFile: open failed");

        struct stat st{};
        if (::fstat(fd, &st) != 0)
        {
            const int err = errno;
            ::close(fd);
            throw std::system_error{err, std::generic_category(),
                                    "readBinaryFile: fstat failed"};
        }

        std::vector<std::byte> result(static_cast<std::size_t>(st.st_size));
        std::size_t read = 0;
        while (read < result.size())
        {
            const ssize_t n = ::read(fd, result.data() + read, result.size() - read);
            if (n < 0)
            {
                if (errno == EINTR) continue;
                const int err = errno;
                ::close(fd);
                throw std::system_error{err, std::generic_category(),
                                        "readBinaryFile: read failed"};
            }
            if (n == 0) break;
            read += static_cast<std::size_t>(n);
        }
        ::close(fd);
        result.resize(read);
        return result;
#endif
    }

    std::string readTextFile(const Path& path)
    {
        const auto bytes = readBinaryFile(path);
        std::string result;
        result.resize(bytes.size());
        std::memcpy(result.data(), bytes.data(), bytes.size());
        return result;
    }
}
