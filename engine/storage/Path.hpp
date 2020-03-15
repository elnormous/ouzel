// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>

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
#elif defined(__unix__) || defined(__APPLE__)
#  include <sys/stat.h>
#endif

namespace ouzel
{
    namespace storage
    {
        class Path final
        {
        public:
#if defined(_WIN32)
            static constexpr char directorySeparator = '\\';
            using Char = wchar_t;
            using String = std::wstring;
#elif defined(__unix__) || defined(__APPLE__)
            static constexpr char directorySeparator = '/';
            using Char = char;
            using String = std::string;
#endif

            Path() = default;
            Path(const std::string& p):
                path(convertToNative(p))
            {
            }

            operator std::string() const
            {
                return convertToUniversal(path);
            }

            Path& operator+=(const Path& p)
            {
                path += p.path;
                return *this;
            }

            Path& operator+=(const std::string& p)
            {
                path += convertToNative(p);
                return *this;
            }

            Path operator+(const Path& p)
            {
                Path result = *this;
                result.path += p.path;
                return result;
            }

            Path operator+(const std::string& p)
            {
                Path result = *this;
                result.path += convertToNative(p);
                return result;
            }

            const String& getNative() const noexcept
            {
                return path;
            }

            Path getExtension() const
            {
                const std::size_t pos = path.find_last_of(Char('.'));

                Path result;

                if (pos != std::string::npos)
                    result.path = path.substr(pos + 1);
                else
                    result.path = path;

                return result;
            }

            Path getFilename() const
            {
                const std::size_t pos = path.find_last_of(directorySeparator);

                Path result;

                if (pos != String::npos)
                    result.path = path.substr(pos + 1);
                else
                    result.path = path;

                return result;
            }

            Path getStem() const
            {
                const std::size_t directoryPos = path.find_last_of(directorySeparator);
                const std::size_t startPos = directoryPos == String::npos ? 0 : directoryPos + 1;
                const std::size_t extensionPos = path.find(Char('.'), startPos);
                const std::size_t endPos = extensionPos == String::npos ? path.size() : extensionPos;

                Path result;
                result.path = path.substr(startPos, endPos - startPos);
                return result;
            }

            Path getDirectory() const
            {
                const std::size_t pos = path.find_last_of(directorySeparator);

                Path result;
                if (pos != String::npos)
                    result.path = path.substr(0, pos);

                return result;
            }

            Path getRoot() const
            {
                Path result;
#if defined(_WIN32)
                if (path.size() >= 2 &&
                    ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                    path[1] == L':')
                    result.path = path.substr(0, 2);
#elif defined(__unix__) || defined(__APPLE__)
                if (path.size() >= 1 && path[0] == '/') result.path = '/';
#endif
                return result;
            }

            bool isDirectory() const noexcept
            {
#if defined(_WIN32)
                const DWORD attributes = GetFileAttributesW(path.c_str());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                    return false;

                return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif defined(__unix__) || defined(__APPLE__)
                struct stat buf;
                if (stat(path.c_str(), &buf) == -1)
                    return false;

                return (buf.st_mode & S_IFMT) == S_IFDIR;
#endif
            }

            bool isFile() const noexcept
            {
#if defined(_WIN32)
                const DWORD attributes = GetFileAttributesW(path.c_str());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                    return false;

                return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#elif defined(__unix__) || defined(__APPLE__)
                struct stat buf;
                if (stat(path.c_str(), &buf) == -1)
                    return false;

                return (buf.st_mode & S_IFMT) == S_IFREG;
#endif
            }

            bool isAbsolute() const noexcept
            {
#if defined(_WIN32)
                return path.size() >= 2 &&
                    ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                    path[1] == L':';
#elif defined(__unix__) || defined(__APPLE__)
                return path.size() >= 1 && path[0] == '/';
#endif
            }

#if defined(_WIN32)
            std::chrono::system_clock::time_point getAccessTime() const
            {
                HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");

                FILETIME time;
                auto ret = GetFileTime(file, nullptr, &time, nullptr);
                CloseHandle(file);
                if (!ret)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file time");

                using hundrednanoseconds = std::chrono::duration<std::int64_t, std::ratio_multiply<std::hecto, std::nano>>;

                auto t = hundrednanoseconds{
                    ((static_cast<std::uint64_t>(time.dwHighDateTime) << 32) |
                     static_cast<std::uint64_t>(time.dwLowDateTime)) - 116444736000000000LL
                };

                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(t)};
            }

            std::chrono::system_clock::time_point getModifyTime() const
            {
                HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");

                FILETIME time;
                auto ret = GetFileTime(file, nullptr, nullptr, &time);
                CloseHandle(file);
                if (!ret)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file time");

                using hundrednanoseconds = std::chrono::duration<std::int64_t, std::ratio_multiply<std::hecto, std::nano>>;

                auto t = hundrednanoseconds{
                    ((static_cast<std::uint64_t>(time.dwHighDateTime) << 32) |
                     static_cast<std::uint64_t>(time.dwLowDateTime)) - 116444736000000000LL
                };

                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(t)};
            }
#elif defined(__unix__) || defined(__APPLE__)
            std::chrono::system_clock::time_point getAccessTime() const
            {
                struct stat s;
                if (lstat(path.c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file stats");

#  if defined(__APPLE__)
                auto nanoseconds = std::chrono::seconds{s.st_atimespec.tv_sec} +
                    std::chrono::nanoseconds{s.st_atimespec.tv_nsec};
#  else
                auto nanoseconds = std::chrono::seconds{s.st_atim.tv_sec} +
                    std::chrono::nanoseconds{s.st_atim.tv_nsec};
#  endif
                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds)};
            }

            std::chrono::system_clock::time_point getModifyTime() const
            {
                struct stat s;
                if (lstat(path.c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file stats");

#  if defined(__APPLE__)
                auto nanoseconds = std::chrono::seconds{s.st_mtimespec.tv_sec} +
                    std::chrono::nanoseconds{s.st_mtimespec.tv_nsec};
#  else
                auto nanoseconds = std::chrono::seconds{s.st_mtim.tv_sec} +
                    std::chrono::nanoseconds{s.st_mtim.tv_nsec};
#  endif
                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds)};
            }
#endif

        private:
            static std::string toUtf8(const std::wstring& p)
            {
				std::string s;

				for (wchar_t w : p)
				{
					if (w <= 0x7F)
						s.push_back(static_cast<char>(w));
					else if (w <= 0x7FF)
					{
						s.push_back(static_cast<char>(0xC0 | ((w >> 6) & 0x1F)));
						s.push_back(static_cast<char>(0x80 | (w & 0x3F)));
					}
					else if (w <= 0xFFFF)
                    {
                        s.push_back(static_cast<char>(0xE0 | ((w >> 12) & 0x0F)));
                        s.push_back(static_cast<char>(0x80 | ((w >> 6) & 0x3F)));
                        s.push_back(static_cast<char>(0x80 | (w & 0x3F)));
                    }
#if WCHAR_MAX > 0xFFFF
                    else
                    {
                        s.push_back(static_cast<char>(0xF0 | ((w >> 18) & 0x07)));
                        s.push_back(static_cast<char>(0x80 | ((w >> 12) & 0x3F)));
                        s.push_back(static_cast<char>(0x80 | ((w >> 6) & 0x3F)));
                        s.push_back(static_cast<char>(0x80 | (w & 0x3F)));
                    }
#endif
				}

                return s;
            }

#if defined(_WIN32)
            static std::wstring toWchar(const std::string& p)
            {
                std::wstring s;
                
				for (auto i = p.begin(); i != p.end(); ++i)
				{
					char32_t cp = *i & 0xFF;

					if (cp <= 0x7F) // length = 1
					{
						// do nothing
					}
					else if ((cp >> 5) == 0x6) // length = 2
					{
						if (++i == p.end())
							throw std::runtime_error("Invalid UTF-8 string");
						cp = ((cp << 6) & 0x7FF) + (*i & 0x3F);
					}
					else if ((cp >> 4) == 0xE) // length = 3
					{
						if (++i == p.end())
							throw std::runtime_error("Invalid UTF-8 string");
						cp = ((cp << 12) & 0xFFFF) + (((*i & 0xFF) << 6) & 0x0FFF);
						if (++i == p.end())
							throw std::runtime_error("Invalid UTF-8 string");
						cp += *i & 0x3F;
					}
					else if ((cp >> 3) == 0x1E) // length = 4
					{
						if (++i == p.end())
							throw std::runtime_error("Invalid UTF-8 string");
						cp = ((cp << 18) & 0x1FFFFF) + (((*i & 0xFF) << 12) & 0x3FFFF);
						if (++i == p.end())
							throw std::runtime_error("Invalid UTF-8 string");
						cp += ((*i & 0xFF) << 6) & 0x0FFF;
						if (++i == p.end())
							throw std::runtime_error("Invalid UTF-8 string");
						cp += (*i) & 0x3F;
					}

					if (cp > WCHAR_MAX)
						throw std::runtime_error("Unsupported UTF-8 character");

					s.push_back(static_cast<wchar_t>(cp));
				}

                return s;
            }

            static std::wstring convertToNative(const std::string& p)
            {
                std::wstring result = toWchar(p);

                for (auto& c : result)
                    if (c == L'/') c = L'\\';

                return result;
            }

            static std::string convertToUniversal(const std::wstring& p)
            {
                std::string result = toUtf8(p);

                for (auto& c : result)
                    if (c == directorySeparator) c = '/';

                return result;
            }
#elif defined(__unix__) || defined(__APPLE__)
            static const std::string& convertToNative(const std::string& p)
            {
                return p;
            }

            static const std::string& convertToUniversal(const std::string& p)
            {
                return p;
            }
#endif

            String path;
        };
    } // namespace storage
} // namespace ouzel

#endif
