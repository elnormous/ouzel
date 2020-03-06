// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

#include <stdexcept>
#include <string>

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
                const int size = WideCharToMultiByte(CP_UTF8, 0, p.c_str(), static_cast<int>(p.size()), nullptr, 0, nullptr, nullptr);
                if (size == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                std::string s;
                s.resize(size);
                if (WideCharToMultiByte(CP_UTF8, 0, p.c_str(), static_cast<int>(p.size()), s.data(), static_cast<int>(s.size()), nullptr, nullptr) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                return s;
            }

            static std::wstring toWchar(const std::string& p)
            {
                const int size = MultiByteToWideChar(CP_UTF8, 0, p.c_str(), static_cast<int>(p.size()), nullptr, 0);
                if (size == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

                std::wstring s;
                s.resize(static_cast<size_t>(size));
                if (MultiByteToWideChar(CP_UTF8, 0, p.c_str(), static_cast<int>(p.size()), s.data(), static_cast<int>(s.size())) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

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
