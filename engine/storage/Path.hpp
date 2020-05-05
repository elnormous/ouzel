// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>
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
#elif defined(__unix__) || defined(__APPLE__)
#  include <sys/stat.h>
#endif

namespace ouzel
{
    namespace storage
    {
        class FileTime final
        {
        public:
#if defined(_WIN32)
            using Type = FILETIME;
#elif defined(__unix__) || defined(__APPLE__)
            using Type = timespec;
#endif
            FileTime() noexcept = default;
            FileTime(const Type& t) noexcept: time{t} {}

            operator Type() const noexcept { return time; }

            operator std::chrono::system_clock::time_point() const noexcept
            {
#if defined(_WIN32)
                using hundrednanoseconds = std::chrono::duration<std::int64_t, std::ratio_multiply<std::hecto, std::nano>>;

                auto t = hundrednanoseconds{
                    ((static_cast<std::uint64_t>(time.dwHighDateTime) << 32) |
                     static_cast<std::uint64_t>(time.dwLowDateTime)) - 116444736000000000LL
                };

                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(t)};
#elif defined(__unix__) || defined(__APPLE__)
                auto nanoseconds = std::chrono::seconds{time.tv_sec} +
                    std::chrono::nanoseconds{time.tv_nsec};

                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds)};
#endif
            }

            bool operator==(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime &&
                    time.dwLowDateTime == other.time.dwLowDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec &&
                    time.tv_nsec == other.time.tv_nsec;
#endif
            }

            bool operator>(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime > other.time.dwLowDateTime :
                    time.dwHighDateTime > other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec > other.time.tv_nsec :
                    time.tv_sec > other.time.tv_sec;
#endif
            }

            bool operator<(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime < other.time.dwLowDateTime :
                    time.dwHighDateTime < other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec < other.time.tv_nsec :
                    time.tv_sec < other.time.tv_sec;
#endif
            }

            bool operator>=(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime >= other.time.dwLowDateTime :
                    time.dwHighDateTime > other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec >= other.time.tv_nsec :
                    time.tv_sec > other.time.tv_sec;
#endif
            }

            bool operator<=(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime <= other.time.dwLowDateTime :
                    time.dwHighDateTime < other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec <= other.time.tv_nsec :
                    time.tv_sec < other.time.tv_sec;
#endif
            }

        private:
            Type time;
        };

        class Path final
        {
        public:
            enum class Format
            {
                Generic,
                Native
            };

            enum class Type
            {
                NotFound,
                Regular,
                Directory,
                Symlink,
                Block,
                Character,
                Fifo,
                Socket,
                Unknown
            };

#if defined(_WIN32)
            using Char = wchar_t;
            using String = std::wstring;
            static constexpr Char preferredSeparator = '\\';
            static constexpr const Char* parentDirectory = L"..";
            static constexpr const Char* currentDirectory = L".";
#elif defined(__unix__) || defined(__APPLE__)
            using Char = char;
            using String = std::string;
            static constexpr Char preferredSeparator = '/';
            static constexpr const Char* parentDirectory = "..";
            static constexpr const Char* currentDirectory = ".";
#endif

            Path() = default;

            Path(const Path& p):
                path(p.path)
            {
            }

            Path(Path&& p) noexcept:
                path(std::move(p.path))
            {
            }

            template <class Source>
            Path(const Source& p, Format format = Format::Generic):
                path(format == Format::Generic ? convertToNative(p) : encode(p))
            {
            }

            Path& operator=(const Path& other)
            {
                if (this == &other) return *this;
                path = other.path;
                return *this;
            }

            Path& operator=(Path&& other) noexcept
            {
                if (this == &other) return *this;
                path = std::move(other.path);
                return *this;
            }

            operator std::string() const
            {
                return convertToUtf8(path);
            }

            bool operator==(const Path& p) const noexcept
            {
                return path == p.path;
            }

            template <class Source>
            bool operator==(const Source& p) const
            {
                return path == convertToNative(p);
            }

            Path& operator+=(const Path& p)
            {
                path += p.path;
                return *this;
            }

            template <class Source>
            Path& operator+=(const Source& p)
            {
                path += convertToNative(p);
                return *this;
            }

            Path& operator/=(const Path& p)
            {
                if (!path.empty() && !isDirectorySeparator(path.back()))
                    path += preferredSeparator;
                path += p.path;
                return *this;
            }

            template <class Source>
            Path& operator/=(const Source& p)
            {
                if (!path.empty() && !isDirectorySeparator(path.back()))
                    path += preferredSeparator;
                path += convertToNative(p);
                return *this;
            }

            Path operator+(const Path& p) const
            {
                Path result = *this;
                result.path += p.path;
                return result;
            }

            template <class Source>
            Path operator+(const Source& p) const
            {
                Path result = *this;
                result.path += convertToNative(p);
                return result;
            }

            Path operator/(const Path& p) const
            {
                Path result = *this;
                if (!result.path.empty() && !isDirectorySeparator(result.path.back()))
                    result.path += preferredSeparator;
                result.path += p.path;
                return result;
            }

            template <class Source>
            Path operator/(const Source& p) const
            {
                Path result = *this;
                if (!result.path.empty() && !isDirectorySeparator(result.path.back()))
                    result.path += preferredSeparator;
                result.path += convertToNative(p);
                return result;
            }

            bool isEmpty() const noexcept
            {
                return path.empty();
            }

            const String& getNative() const noexcept
            {
                return path;
            }

            std::string getGeneric() const
            {
                return convertToGeneric(path);
            }

            Path getExtension() const
            {
                const std::size_t pos = path.rfind(Char('.'));
                Path result;
                result.path = pos != std::string::npos ? path.substr(pos + 1) : String{};
                return result;
            }

            template <class Source>
            void replaceExtension(const Source& extension)
            {
                const std::size_t pos = path.rfind(Char('.'));
                if (pos != std::string::npos)
                    path.resize(pos);

                path.push_back(Char('.'));
                path += convertToNative(extension);
            }

            Path getFilename() const
            {
                const std::size_t position = findLastDirectorySeparator(path);
                Path result;
                result.path = position != String::npos ? path.substr(position + 1) : path;
                return result;
            }

            template <class Source>
            void replaceFilename(const Source& filename)
            {
                const std::size_t position = findLastDirectorySeparator(path);
                if (position != std::string::npos)
                    path.resize(position + 1);
                else
                    path.clear();

                path += convertToNative(filename);
            }

            Path getStem() const
            {
                const std::size_t directoryPosition = findLastDirectorySeparator(path);
                const std::size_t startPosition = directoryPosition == String::npos ? 0 : directoryPosition + 1;
                const std::size_t extensionPos = path.find(Char('.'), startPosition);
                const std::size_t endPosition = extensionPos == String::npos ? path.size() : extensionPos;

                Path result;
                result.path = path.substr(startPosition, endPosition - startPosition);
                return result;
            }

            Path getDirectory() const
            {
                Path result;
                const std::size_t position = findLastDirectorySeparator(path);
                if (position != String::npos)
                    result.path = path.substr(0, position);

                return result;
            }

            Path getRoot() const
            {
                Path result;
#if defined(_WIN32)
                if (path.size() >= 2 &&
                    ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                    path[1] == L':')
                    result.path = {path[0], ':'};
#elif defined(__unix__) || defined(__APPLE__)
                if (path.size() >= 1 && path[0] == '/') result.path = '/';
#endif
                return result;
            }

            void normalize()
            {
                std::vector<String> parts;
                String newPath;
                String::size_type previousPosition = 0;

#if defined(_WIN32)
                if (path.size() >= 2 &&
                    ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                    path[1] == L':')
                {
                    parts.push_back({path[0], ':'});
                    previousPosition = 2;

                    if (path.size() >= 3)
                        parts.push_back(L"\\");
                }
#elif defined(__unix__) || defined(__APPLE__)
                if (path.size() >= 1 && path[0] == '/')
                {
                    parts.push_back("/");
                    previousPosition = 1;
                }
#endif

                while (previousPosition < path.length())
                {
                    if (isDirectorySeparator(path[previousPosition])) ++previousPosition;

                    const std::size_t position = findFirstDirectorySeparator(path, previousPosition);
                    const auto endPosition = position != String::npos ? position : path.length();
                    const String currentPart = path.substr(previousPosition, endPosition - previousPosition);

                    if (currentPart == parentDirectory)
                    {
                        if (parts.empty())
                            parts.push_back(currentPart);
                        else
                        {
                            const auto& previousPart = parts.back();
                            if (previousPart != parentDirectory &&
                                previousPart.length() != 1 && !isDirectorySeparator(previousPart.back()))
                                parts.pop_back();
                        }
                    }
                    else if (currentPart.empty())
                    {
                        if (position == String::npos) // the last part
                            parts.push_back(currentPart);
                    }
                    else if (currentPart != currentDirectory)
                        parts.push_back(currentPart);

                    previousPosition = endPosition;
                }

                for (const auto& part : parts)
                {
                    if (!newPath.empty() && !isDirectorySeparator(newPath.back()) &&
                        (part.length() != 1 || !isDirectorySeparator(part.back())))
                        newPath += preferredSeparator;

                    newPath += part;
                }

                path = newPath;
            }

            Path getNormal() const
            {
                Path result = *this;
                result.normalize();
                return result;
            }

            Type getType() const noexcept
            {
#if defined(_WIN32)
                const DWORD attributes = GetFileAttributesW(path.c_str());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                    return Type::NotFound;

                if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
                    return Type::Symlink;
                else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                    return Type::Directory;
                else
                    return Type::Regular;
#elif defined(__unix__) || defined(__APPLE__)
                struct stat buf;
                if (stat(path.c_str(), &buf) == -1)
                    return Type::NotFound;

                if ((buf.st_mode & S_IFMT) == S_IFREG)
                    return Type::Regular;
                else if ((buf.st_mode & S_IFMT) == S_IFDIR)
                    return Type::Directory;
                else if ((buf.st_mode & S_IFMT) == S_IFLNK)
                    return Type::Symlink;
                else if ((buf.st_mode & S_IFMT) == S_IFBLK)
                    return Type::Block;
                else if ((buf.st_mode & S_IFMT) == S_IFCHR)
                    return Type::Character;
                else if ((buf.st_mode & S_IFMT) == S_IFIFO)
                    return Type::Fifo;
                else if ((buf.st_mode & S_IFMT) == S_IFSOCK)
                    return Type::Socket;
                else
                    return Type::Unknown;
#endif
            }

            bool exists() const noexcept
            {
                return getType() != Type::NotFound;
            }

            bool isDirectory() const noexcept
            {
                return getType() == Type::Directory;
            }

            bool isRegular() const noexcept
            {
                return getType() == Type::Regular;
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

            FileTime getAccessTime() const
            {
#if defined(_WIN32)
                HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");

                FILETIME time;
                auto ret = GetFileTime(file, nullptr, &time, nullptr);
                CloseHandle(file);
                if (!ret)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file time");

                return FileTime{time};
#elif defined(__unix__) || defined(__APPLE__)
                struct stat s;
                if (lstat(path.c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file stats");

#  if defined(__APPLE__)
                return FileTime{s.st_atimespec};
#  else
                return FileTime{s.st_atim};
#  endif
#endif
            }

            FileTime getModifyTime() const
            {
#if defined(_WIN32)
                HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");

                FILETIME time;
                auto ret = GetFileTime(file, nullptr, nullptr, &time);
                CloseHandle(file);
                if (!ret)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file time");

                return FileTime{time};
#elif defined(__unix__) || defined(__APPLE__)
                struct stat s;
                if (lstat(path.c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file stats");

#  if defined(__APPLE__)
                return FileTime{s.st_mtimespec};
#  else
                return FileTime{s.st_mtim};
#  endif
#endif
            }

        private:
            static constexpr bool isDirectorySeparator(Char c) noexcept
            {
#if defined(_WIN32)
                return c == L'\\' || c == L'/';
#elif defined(__unix__) || defined(__APPLE__)
                return c == '/';
#endif
            }

            static std::size_t findLastDirectorySeparator(const String& str,
                                                          std::size_t position = String::npos) noexcept
            {
#if defined(_WIN32)
                return str.find_last_of(L"\\/", position);
#elif defined(__unix__) || defined(__APPLE__)
                return str.rfind('/', position);
#endif
            }

            static std::size_t findFirstDirectorySeparator(const String& str,
                                                           std::size_t position = 0) noexcept
            {
#if defined(_WIN32)
                return str.find_first_of(L"\\/", position);
#elif defined(__unix__) || defined(__APPLE__)
                return str.find('/', position);
#endif
            }

            static const std::string& convertToUtf8(const std::string& p) noexcept
            {
                return p;
            }

            static std::string convertToUtf8(wchar_t c)
            {
                std::string s;

                if (c <= 0x7F)
                    s.push_back(static_cast<char>(c));
                else if (c <= 0x7FF)
                {
                    s.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                    s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
                else if (c <= 0xFFFF)
                {
                    s.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                    s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                    s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
#if WCHAR_MAX > 0xFFFF
                else
                {
                    s.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                    s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                    s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                    s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
#endif

                return s;
            }

            static std::string convertToUtf8(const std::wstring& p)
            {
				std::string s;

				for (wchar_t c : p)
				{
					if (c <= 0x7F)
						s.push_back(static_cast<char>(c));
					else if (c <= 0x7FF)
					{
						s.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
						s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
					}
					else if (c <= 0xFFFF)
                    {
                        s.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                        s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                        s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                    }
#if WCHAR_MAX > 0xFFFF
                    else
                    {
                        s.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                        s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                        s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                        s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                    }
#endif
				}

                return s;
            }

#if defined(_WIN32)
            static std::wstring convertToWchar(const std::string& p)
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

            static std::wstring convertToNative(char c)
            {
                wchar_t result = static_cast<wchar_t>(c);
                if (result == '/') result = '\\';
                return std::wstring{result};
            }

            static std::wstring convertToNative(const std::string& p)
            {
                std::wstring result = convertToWchar(p);

                for (auto& c : result) if (c == L'/') c = L'\\';
                return result;
            }

            static std::wstring convertToNative(wchar_t c)
            {
                if (c == L'/') c = L'\\';
                return std::wstring{c};
            }

            static std::wstring convertToNative(const std::wstring& p)
            {
                std::wstring result = p;

                for (auto& c : result) if (c == L'/') c = L'\\';
                return result;
            }

            static std::string convertToGeneric(const std::wstring& p)
            {
                std::string result = convertToUtf8(p);

                for (auto& c : result)
                    if (c == '\\') c = '/';

                return result;
            }

            static std::wstring encode(char c)
            {
                return std::wstring{static_cast<wchar_t>(c)};
            }

            static std::wstring encode(const std::string& p)
            {
                return convertToWchar(p);
            }

            static std::wstring encode(wchar_t c)
            {
                return std::wstring{c};
            }

            static const std::wstring& encode(const std::wstring& p) noexcept
            {
                return p;
            }
#elif defined(__unix__) || defined(__APPLE__)
            static std::string convertToNative(char c)
            {
                return std::string{c};
            }

            static const std::string& convertToNative(const std::string& p) noexcept
            {
                return p;
            }

            static std::string convertToNative(wchar_t c)
            {
                return convertToUtf8(c);
            }

            static std::string convertToNative(const std::wstring& p)
            {
                return convertToUtf8(p);
            }

            static const std::string& convertToGeneric(const std::string& p) noexcept
            {
                return p;
            }

            static std::string encode(char c)
            {
                return std::string{c};
            }

            static const std::string& encode(const std::string& p) noexcept
            {
                return p;
            }

            static std::string encode(wchar_t c)
            {
                return convertToUtf8(c);
            }

            static std::string encode(const std::wstring& p)
            {
                return convertToUtf8(p);
            }
#endif

            String path;
        };
    } // namespace storage
} // namespace ouzel

#endif
