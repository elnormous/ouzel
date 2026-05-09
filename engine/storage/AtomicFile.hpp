#ifndef FLUXION_STORAGE_ATOMICFILE_HPP
#define FLUXION_STORAGE_ATOMICFILE_HPP

#include <cstddef>
#include <string>
#include <vector>
#include "Path.hpp"

namespace fluxion::storage
{
    // Cross-platform atomic file write. Strategy:
    //   1. Write the new contents to `<path>.tmp` and force them to disk
    //      (FlushFileBuffers on Windows, fsync on POSIX).
    //   2. Atomically replace `<path>` by renaming the temp file:
    //        Windows: MoveFileExW + MOVEFILE_REPLACE_EXISTING + MOVEFILE_WRITE_THROUGH
    //        POSIX:   rename(2)  (POSIX guarantees the swap is atomic)
    //
    // Crash-safety: a crash after step 1 leaves the original file intact and
    // a stale .tmp; a crash during step 2 either keeps the old contents or
    // gets the new ones, never a half-written file.
    //
    // Platform notes:
    //   * Emscripten goes through the POSIX path; the in-process IDBFS / MEMFS
    //     honour rename atomicity within a single process, but persistence to
    //     IndexedDB happens asynchronously and is outside our control.
    //   * iOS / tvOS sandbox: the destination directory must be writable
    //     (Documents, Caches, or app group container). Read-only bundles
    //     will fail with errno = EACCES.
    //   * Android scoped storage (API 30+): use app-private storage via
    //     getStorageDirectory(); MediaStore paths will fail with EACCES.
    //
    // Throws std::system_error with the platform errno/HRESULT on any I/O
    // failure; the .tmp file is best-effort cleaned up before the throw.
    void atomicWriteFile(const Path& path, const void* data, std::size_t size);

    inline void atomicWriteFile(const Path& path, const std::string& data)
    {
        atomicWriteFile(path, data.data(), data.size());
    }

    inline void atomicWriteFile(const Path& path, const std::vector<std::byte>& data)
    {
        atomicWriteFile(path, data.data(), data.size());
    }

    // Direct-path file read, distinct from FileSystem::readFile which goes
    // through the resource search path. Throws std::system_error on any
    // failure (missing file, permission, partial read).
    std::vector<std::byte> readBinaryFile(const Path& path);

    // Convenience: read as text (no encoding conversion). The returned
    // string contains exactly the file's bytes — no BOM stripping, no
    // newline normalisation.
    std::string readTextFile(const Path& path);
}

#endif // FLUXION_STORAGE_ATOMICFILE_HPP
