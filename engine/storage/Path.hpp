// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

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
            using String = std::wstring;
#elif defined(__unix__) || defined(__APPLE__)
            static constexpr char directorySeparator = '/';
            using String = std::string;
#endif

            Path() = default;
            Path(const std::string& p):
#if defined(_WIN32)
                path(convertToNative(toWchar(p)))
#elif defined(__unix__) || defined(__APPLE__)
                path(p)
#endif
            {
                // TODO: normalize
            }

            Path(const std::wstring& p):
#if defined(_WIN32)
                path(convertToNative(p))
#elif defined(__unix__) || defined(__APPLE__)
                path(toUtf8(p))
#endif
            {
                // TODO: normalize
            }

            operator std::string() const
            {
#if defined(_WIN32)
                return toUtf8(convertToUniversal(path));
#elif defined(__unix__) || defined(__APPLE__)
                return path;
#endif
            }

            const String& getNative() const noexcept
            {
                return path;
            }

            Path getExtensionPart() const
            {
                const size_t pos = path.find_last_of('.');

                if (pos != std::string::npos)
                    return path.substr(pos + 1);

                return String();
            }

            Path getFilenamePart() const
            {
                const size_t pos = path.find_last_of(directorySeparator);

                if (pos != String::npos)
                    return path.substr(pos + 1);
                else
                    return path;
            }

            Path getDirectoryPart() const
            {
                const size_t pos = path.find_last_of(directorySeparator);

                if (pos != String::npos)
                    return path.substr(0, pos);

                return String();
            }

            bool isDirectory() const
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

            bool isFile() const
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

#if defined(_WIN32)
            static std::wstring convertToNative(const std::wstring& p)
            {
                std::wstring result = p;

                for (auto& c : result)
                    if (c == L'/') c = L'\\';

                return result;
            }

            static std::wstring convertToUniversal(const std::wstring& p)
            {
                std::wstring result = p;

                for (auto& c : result)
                    if (c == L'\\') c = L'/';

                return result;
            }
#endif

            String path;
        };
    } // namespace storage
} // namespace ouzel

#endif
