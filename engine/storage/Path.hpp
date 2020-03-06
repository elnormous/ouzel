// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

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
#elif defined(__unix__) || defined(__APPLE__)
            static constexpr char directorySeparator = '/';
#endif

            Path() = default;
            Path(const std::string& p):
#if defined(_WIN32)
                path(toWchar(p))
#elif defined(__unix__) || defined(__APPLE__)
                path(p)
#endif
            {
                // TODO: normalize
            }

            operator std::string()
            {
#if defined(_WIN32)
                return toUtf8(path);
#elif defined(__unix__) || defined(__APPLE__)
                return path;
#endif
            }

            bool isRelative() const noexcept
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
#if defined(_WIN32)
            static std::string toUtf8(const std::wstring& p)
            {
				std::string s;

				for (wchar_t cp : p)
				{
					if (cp == L'\\') cp = '/';

					if (cp <= 0x7F)
						s.push_back(static_cast<char>(cp));
					else if (cp <= 0x7FF)
					{
						s.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
						s.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
					}
					else
					{
						s.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
						s.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
						s.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
					}
				}

                return s;
            }

            static std::wstring toWchar(const std::string& p)
            {
                std::wstring s;
                
				for (auto i = p.begin(); i != p.end(); ++i)
				{
					wchar_t cp = *i & 0xFF;

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
					else if ((cp >> 4) == 0xE || // length = 3
						(cp >> 3) == 0x1E) // length = 4
						throw std::runtime_error("Unsupported UTF-8 character");

					if (cp == L'/') cp = L'\\';
					s.push_back(cp);
				}

                return s;
            }

            std::wstring path;
#elif defined(__unix__) || defined(__APPLE__)
            std::string path;
#endif
        };
    } // namespace storage
} // namespace ouzel

#endif
